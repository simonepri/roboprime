/**
 * Part of RoboPrime Firmware.
 *
 * AnimHelper.cpp
 * Utility used to create FW animation.
 *
 * RoboPrime Firmware, (https://github.com/simonepri/RoboPrime)
 * Copyright (c) 2015, Simone Primarosa,(http://simoneprimarosa.com)
 *
 * Licensed under The MIT License
 * Redistribution of file must retain the above copyright notice.
 *
 * @copyright     Copyright (c) 2015, Simone Primarosa,(http://simoneprimarosa.com)
 * @link          (https://github.com/simonepri/RoboPrime)
 * @since         0.0.0
 * @require       SerialServo
 * @license       MIT License (https://opensource.org/licenses/MIT)
 */

/*
 * PURPOSE:
 *
 * This convert an xml file with timing about animation steps into code used by the animation
 * routine in the RoboPrime firmware.
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include "pugixml.hpp"
#include "pugixml.cpp"

bool used[20] = {false};

#define catch_error(suorce_name, error_type, error_desc)                       \
std::cout << "XML [" << suorce_name << "] " << error_type << "." << std::endl; \
std::cout << "Error description:" << error_desc << "." << std::endl;           \

struct movment {
	bool half;
	int idx, start, duration, angle;
};

typedef movment mov;

class movcomp {
	public:
		bool operator() (const mov &lhs, const mov &rhs) const {
			return (lhs.start != rhs.start) ? (lhs.start > rhs.start) : ((lhs.duration != rhs.duration) ? (lhs.duration > rhs.duration) : (lhs.idx > rhs.idx));
		}
};

template<class T> T base_name(T const & path, T const & delims = "/\\") {
	return path.substr(path.find_last_of(delims) + 1);
}
template<class T> T remove_extension(T const & filename) {
	typename T::size_type const p(filename.find_last_of('.'));
	return p > 0 && p != T::npos ? filename.substr(0, p) : filename;
}
std::string idx_name(int idx) {
	switch(idx) {
		case 0: return std::string("PART_ANKLE_X_ROT");
		case 1: return std::string("PART_ANKLE_Y_ROT");
		case 2: return std::string("PART_KNEE_X_ROT");
		case 3: return std::string("PART_HIP_Y_ROT");
		case 4: return std::string("PART_HIP_X_ROT");
		case 5: return std::string("PART_HIP_Z_ROT");
		case 6: return std::string("PART_SHOULDER_X_ROT");
		case 7: return std::string("PART_SHOULDER_Y_ROT");
		case 8: return std::string("PART_ELBOW_Z_ROT");
		case 9: return std::string("PART_ELBOW_X_ROT");
	}
}
std::string hf_name(int hf) {
	switch(hf) {
		case 0: return std::string("HF_R");
		case 1: return std::string("HF_L");
	}
}


int main() {
	pugi::xml_document anim;
	std::cout << "Hint: Insert a path for a valid .xml file." << std::endl;
	std::priority_queue<mov, std::vector<mov>, movcomp> queue[3];
	std::string source;
	std::string filename;

	while(1) {
		std::cin>>source;
		filename = remove_extension(base_name(source));
		const char * file = source.c_str();
		pugi::xml_parse_result result = anim.load_file(file);
		if(result) {
			std::cout << "XML [" << source << "] parsed without errors." << std::endl;
			break;
		}
		std::cout << "XML [" << source << "] parsed with errors." << std::endl;
		std::cout << "Error description: " << result.description() << std::endl;
		std::cout << "Error offset: " << result.offset << " (error at [..." << result.offset << "]" << std::endl << std::endl;
	}

	std::cout << "XML [" << source << "] Info: Starting sanity check..." << std::endl;
	for(int i = 0; i < 3; i++) {
		pugi::xml_node container;
		if(i == 0) container = anim.child("start");
		else if(i == 1) container = anim.child("loop");
		else if(i == 2) container = anim.child("end");

		for(pugi::xml_node mov_container = container.child("mov"); mov_container; mov_container = mov_container.next_sibling("mov")) {
			std::string half = mov_container.attribute("half").as_string();
			if(half[0] != 'L' && half[0] != 'R') {
				std::cout << "XML [" << source << "] Fatal Error: 'half' attribute can only be 'L' or 'R'." << std::endl;
				return 2+(i*5);
			}
			mov parsed;
			parsed.half = (half[0] == 'L') ? true : false;
			parsed.idx = mov_container.attribute("idx").as_int(-1.0);
			if(parsed.idx < 0 || parsed.idx > 9) {
				std::cout << "XML [" << source << "] Fatal Error: 'idx' attribute have to be in range '0-9'." << std::endl;
				return 3+(i*5);
			}
			parsed.angle = mov_container.attribute("angle").as_int(-1.0);
			if(parsed.angle < 0 || parsed.angle > 1800) {
				std::cout << "XML [" << source << "] Fatal Error: 'angle' attribute have to be in range '0-1800'." << std::endl;
				return 4+(i*5);
			}
			parsed.start = mov_container.attribute("start").as_int(-1.0);
			if(parsed.start < 0 || parsed.start > 60000) {
				std::cout << "XML [" << source << "] Fatal Error: 'start' attribute have to be in range '0-60000'." << std::endl;
				return 5+(i*5);
			}
			parsed.duration = mov_container.attribute("duration").as_int(-1.0);
			if(parsed.duration < 0 || parsed.duration > 60000) {
				std::cout << "XML [" << source << "] Fatal Error: 'duration' attribute have to be in range '0-60000'." << std::endl;
				return 6+(i*5);
			}
			queue[i].push(parsed);
		}
	}
	std::cout << "XML [" << source << "] Info: Sanity check passed!" << std::endl;

	int last_end[2][10] ={{0}};
	std::string output = filename + ".cpp";
	std::ofstream fout(output);
	std::cout << "XML [" << output << "] Info: Starting code generation..." << std::endl;
	if(!fout) {
		std::cout << "XML [" << output << "] Fatal Error: the program does not have write permissions in this folder." << std::endl;
		return 17;
	}
	int steps = 0;
	for(int i = 0; i < 3; i++) {
		int max_end = 0;
		while(!queue[i].empty()) {
			mov actual = queue[i].top();
			queue[i].pop();
			if(actual.start < last_end[actual.half][actual.idx]) {
				std::cout << "XML [" << output << "] Warning: overlapping movement found, it will be ignored." << std::endl;
				continue;
			}
			int dif = actual.start - last_end[actual.half][actual.idx];
			if(dif) {
				fout << "case " << steps << ": _half = " << hf_name(actual.half) <<"; _idx = " << idx_name(actual.idx) << "; _angle = INVALID_BODY_POS; _time = " << dif << "; break;" << std::endl;
				steps++;
			}
			fout << "case " << steps << ": _half = " << hf_name(actual.half) <<"; _idx = " << idx_name(actual.idx) << "; _angle = " << actual.angle << "; _time = " << actual.duration << "; break;" << std::endl;
			steps++;
			last_end[actual.half][actual.idx] = actual.start + actual.duration;
			if(max_end < last_end[actual.half][actual.idx]) max_end = last_end[actual.half][actual.idx];
		}
		for(int j = 0; j < 2; j++) {
			for(int k = 0; k < 10; k++) {
				if(last_end[j][k] != 0) {
					int max_dif = max_end - last_end[j][k];
					if(max_dif) {
						fout << "case " << steps << ": _half = " << hf_name(j) << "; _idx = " << idx_name(k) << "; _angle = INVALID_BODY_POS; _time = " << max_dif << "; break;" << std::endl;
						steps++;
					}
					last_end[j][k] = 0;
				}
			}
		}
		fout<<std::endl;
	}
	fout.close();
	std::cout << "XML [" << output << "] Code generated!" << std::endl;
	return 0;
}
