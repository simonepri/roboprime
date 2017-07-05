# Roboprime 🤖
> Full featured 21 DOF 3D Printed Humanoid Robot based on ATmega328P chip
<p align="center">
  <img src="gallery/hello-world.gif" alt="Roboprime Hello">
</p>
Description coming soon.

# Press
<p>
  <a href="http://explore.makerfairerome.eu/poi/Exhibit_1447"><img src="https://user-images.githubusercontent.com/3505087/27860797-617e0b1a-617e-11e7-9bb1-460a81c064e9.png" alt="Maker Faire" width="128"></a>
  <a href="http://www.ilmessaggero.it/pay/edicola/maker_faire_roma_14_ottobre-2001501.html"> <img src="https://user-images.githubusercontent.com/3505087/27860876-c14fec8e-617e-11e7-8f0e-690555bec853.png" alt="Il Messagero" width="200"></a>
</p>

## Bill of Materials
Amount | Hardware
-----|-------
1 | Arduino Pro Micro (Atmega328p)
1 | HC-05 (Bluetooth)
2 | 74HC4017 (5-stage Johnson decade counter)
1 | MPU-6050 (Gyroscope + Accellerometer UNUSED)
21 | MG90S 9G (Servo motors)
1 | Ad-hoc board (See wiring section)

Amount | Screw
-----|-------
37 | M2 x 4mm
32 | M2 x 14mm
19 | M2 x 20mm

Amount | 3D Model | Amount | 3D Model
-----|-------|-------|-------
38 | [SP_Cover_1_foro](3D/stl/1xSP_Cover_1_foro.stl) |  | 
1 | [MID_Scapole](3D/stl/1xMID_Scapole.stl) |  | 
2 | [MID_Torace](3D/stl/1xMID_Torace.stl) |  | 
1 | [MID_Anca_TOP](3D/stl/1xMID_Anca_TOP.stl) |  | 
2 | [MID_Anca_BTM](3D/stl/1xMID_Anca_BTM.stl) |  | 
1 | [DX_Spalla_A](3D/stl/1xDX_Spalla_A.stl) | 1 | [SX_Spalla_A](3D/stl/1xDX_Spalla_A.stl)
1 | [DX_Spalla_B](3D/stl/1xDX_Spalla_B.stl) | 1 | [SX_Spalla_B](3D/stl/1xDX_Spalla_B.stl)
1 | [DX_Avambraccio_A](3D/stl/1xDX_Avambraccio_A.stl) | 1 | [SX_Avambraccio_A](3D/stl/1xDX_Avambraccio_A.stl)
1 | [DX_Avambraccio_B](3D/stl/1xDX_Avambraccio_B.stl) | 1 | [SX_Avambraccio_B](3D/stl/1xDX_Avambraccio_B.stl)
1 | [DX_Gomito_A](3D/stl/1xDX_Gomito_A.stl) | 1 | [SX_Gomito_A](3D/stl/1xDX_Gomito_A.stl)
1 | [DX_Gomito_B](3D/stl/1xDX_Gomito_B.stl) | 1 | [SX_Gomito_B](3D/stl/1xDX_Gomito_B.stl)
1 | [DX_Braccio](3D/stl/1xDX_Braccio.stl) | 1 | [SX_Braccio](3D/stl/1xDX_Braccio.stl)
1 | [DX_Anca_A](3D/stl/1xDX_Anca_A.stl) | 1 | [SX_Anca_A](3D/stl/1xDX_Anca_A.stl)
1 | [DX_Anca_B](3D/stl/1xDX_Anca_B.stl) | 1 | [SX_Anca_B](3D/stl/1xDX_Anca_B.stl)
1 | [DX_Inguine_A](3D/stl/1xDX_Inguine_A.stl) | 1 | [SX_Inguine_A](3D/stl/1xDX_Inguine_A.stl)
1 | [DX_Inguine_B](3D/stl/1xDX_Inguine_B.stl) | 1 | [SX_Inguine_B](3D/stl/1xDX_Inguine_B.stl)
1 | [DX_Ginocchio_A](3D/stl/1xDX_Ginocchio_A.stl) | 1 | [SX_Ginocchio_A](3D/stl/1xDX_Ginocchio_A.stl)
1 | [DX_Ginocchio_B](3D/stl/1xDX_Ginocchio_B.stl) | 1 | [SX_Ginocchio_B](3D/stl/1xDX_Ginocchio_B.stl)
1 | [DX_Gamba_A](3D/stl/1xDX_Gamba_A.stl) | 1 | [SX_Gamba_A](3D/stl/1xDX_Gamba_A.stl)
1 | [DX_Gamba_B](3D/stl/1xDX_Gamba_B.stl) | 1 | [SX_Gamba_B](3D/stl/1xDX_Gamba_B.stl)
1 | [DX_Caviglia_A](3D/stl/1xDX_Caviglia_A.stl) | 1 | [SX_Caviglia_A](3D/stl/1xDX_Caviglia_A.stl)
1 | [DX_Caviglia_B](3D/stl/1xDX_Caviglia_B.stl) | 1 | [SX_Caviglia_B](3D/stl/1xDX_Caviglia_B.stl)
1 | [DX_Piede_A](3D/stl/1xDX_Piede_A.stl) | 1 | [SX_Piede_A](3D/stl/1xDX_Piede_A.stl)
1 | [DX_Piede_B](3D/stl/1xDX_Piede_B.stl) | 1 | [SX_Piede_B](3D/stl/1xDX_Piede_B.stl)

> SX_* models needs to be mirrored on the correct axis before the printing process.

## Wiring
Coming soon.

## Animations
The firmware contains some basic animations hardcoded inside it.
> Click on the arrow to see an example of the animation!

<details> 
  <summary><b>Forward walk</b> - <i>work-in-progress</i></summary>
  <br/>
  <img src="gallery/walk-animation.gif" alt="Humanoid Forward Walk Animation">
</details>
<details> 
  <summary><b>Backward walk</b> - <i>unavailable</i></summary>
  <br/>
  <img src="https://via.placeholder.com/512x256?text=Not+Implemented" alt="Humanoid Backward Walk Animation">
</details>
<details> 
  <summary><b>Side walk to right</b> - <i>unavailable</i></summary>
  <br/>
  <img src="https://via.placeholder.com/512x256?text=Not+Implemented" alt="Humanoid Side Walk to Right Animation">
</details>
<details> 
  <summary><b>Side walk to left</b> - <i>unavailable</i></summary>
  <br/>
  <img src="https://via.placeholder.com/512x256?text=Not+Implemented" alt="Humanoid Side Walk to Left Animation">
</details>
<details> 
  <summary><b>Clockwise standstill rotation</b> - <i>unavailable</i></summary>
  <br/>
  <img src="https://via.placeholder.com/512x256?text=Not+Implemented" alt="Humanoid Clockwise Standstill Rotation Animation">
</details>
<details> 
  <summary><b>Counterclockwise standstill rotation</b> - <i>unavailable</i></summary>
  <br/>
  <img src="https://via.placeholder.com/512x256?text=Not+Implemented" alt="Humanoid Counterclockwise Standstill Rotation Animation">
</details>
<details> 
  <summary><b>Clockwise curved walk</b> - <i>unavailable</i></summary>
  <br/>
  <img src="https://via.placeholder.com/512x256?text=Not+Implemented" alt="Humanoid Clockwise Curved Walk Animation">
</details>
<details> 
  <summary><b>Counterclockwise curved walk</b> - <i>unavailable</i></summary>
  <br/>
  <img src="https://via.placeholder.com/512x256?text=Not+Implemented" alt="Humanoid Counterclockwise Curved Walk Animation">
</details>
<details> 
  <summary><b>Upward lift</b> - <i>unavailable</i></summary>
  <br/>
  <img src="https://via.placeholder.com/512x256?text=Not+Implemented" alt="Humanoid Upward Lift Animation">
</details>
<details> 
  <summary><b>Downward lift</b> - <i>unavailable</i></summary>
  <br/>
  <img src="https://via.placeholder.com/512x256?text=Not+Implemented" alt="Humanoid Downward Lift Animation">
</details>
<details> 
  <summary><b>Sit down</b> - <i>done</i></summary>
  <br/>
  <img src="gallery/sitdown-animation.gif" alt="Humanoid Sit Down Animation">
</details>
<details> 
  <summary><b>Hello</b> - <i>done</i></summary>
  <br/>
  <img src="gallery/hello-animation.gif" alt="Humanoid Hello Animation">
</details>
<details> 
  <summary><b>F*ck off</b> - <i>done</i></summary>
  <br/>
  <img src="gallery/fuckoff-animation.gif" alt="Humanoid Hello Animation">
</details>

## Wiki
Coming soon

## Authors
Coming soon

## License
Coming soon
