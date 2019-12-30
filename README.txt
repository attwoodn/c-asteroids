Noah Attwood
Asteroids
Summer 2018


============ Compiling ============

the program can be compiled with the following command:
gcc -o asteroids.o asteroids.c -lGL -lGLU -lglut -lm



============= Running =============

please run the program with the following command:
./asteroids.o



============ Gameplay =============

The asteroids game starts in a "safe" mode where no asteroids are generated. The ship can be controlled while in this mode.
The game makes use of the following keys:

s - start the game (when in the safe mode)
c - switch from using polgonal asteroids to circular asteroids and back again
p - pause the game
q - quit the game

space - fire a photon laser shot
arrow keys - control the ship


When the ship is destroyed, it is respawned in the center of the screen with a few seconds of invincibility. The ship 
flashes while the invincibility is active to convey this status to the player.



========== Future Plans ==========

The "safe" mode was planned to be a main menu screen where the user could choose options by shooting photons at onscreen text,
such as "start game", and "controls". I hope to implement this functionality later on. 

Additionally, dust particle generation upon asteroid and ship destruction was not implemented in this version. This is something
that would have added to the sense of quality of the game. I hope to add this feature later. 
