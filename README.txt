# Coffee Machine Project

This project offers an interactive user interface (UI) and backend code for a coffee machine. 


## Getting Started

The user interface consists of four screens:

-	Start Screen: 

	Upon touch, the user is navigated to the Size Screen. In the top left corner, the "Lang" button allows the user to set the language to either English or French.


-	Size Screen: 

	This screen allows the user to select the size of their coffee. After selection, the user is directed to the Confirmation Screen.


-	Confirmation Screen: 

	This screen displays the selected coffee size. If the user has made an incorrect selection, they can return to the Size Screen to correct it. Otherwise, they can click on "BREW" to proceed to the Brewing Screen.


-	Brewing Screen: 

	This screen has a set timeout duration. If no message is received from the backend within 10 seconds, an error message is displayed. Otherwise, content corresponding to the backend message is displayed. After displaying the message, the screen will return to the Start Screen after 7 seconds.


Note: If there is no interaction within 10 seconds, the interface automatically navigates back to the Start Screen.


### Built With

frontend: Crank Storyboard, LUA
backend: Microsoft Visual Studio, C++
communication protocol: Storyboard IO
