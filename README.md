# OpenGL_Game_Insanity_Shaders
A first-person navigable game that simulates the effects of fear and insanity in abstract ways.

Instructions: Launch the executable and use the mouse to control the view

Completion time: ~38 hours

Notes on the release: As it stands, this final project shows the minimum implementation of a castle-like map with a first-person camera. The reason for the large amount of time spent on this project is because since I had previously relied on example files to build homeworks, I felt that in order to get to know OpenGL 4 and actually understand the use of buffer objects, I would build everything from scratch using SDL2 and OpenGL 4 including setting up a perspective camera and first person view. Once I got the windowing to work, the most time-consuming endeavor was writing the OBJ loader from scratch. I utilized a number of different vectors in order to sort UVs, vertices, and indexes in 2-dimensional vectors to keep them in an order that OpenGL would understand when placed in the buffer. The trickiest part was getting the indices to work right, since there are 2 ways to load in models. Either you can store a new vertex for each index regardless of whether it is a duplicate, or you can delete duplicates and have indexes point to shared vertices. I decided to discard duplicated vertices and only duplicate them when one has multiple texture coordinates. While this did save memory and loading times, ensuring that the indices were correct was a huge pain. The castle models and scenery shown was created by me, and I compiled all of the textures and reassigned UVs used into one giant "megatexture" so that a single model and single texture could be loaded into the program.

- Dependencies

SDL image
	sudo apt-get install libsdl2-image-dev
SDL2
	sudo apt-get install libsdl2-dev
GLEW
	sudo apt-get install libglew-dev
	
