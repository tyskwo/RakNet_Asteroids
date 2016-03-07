# Asteroids #
##### Cooperative, asteroid-smashing fun. #####

[David Hartman](https://twitter.com/wednesdayscones) and [Ty Wood](http://tyskwo.com)

[Repository URL](https://tyskwo@bitbucket.org/tyskwo/asteroids_hartman_wood.git)

* * * 

### Project Synopsis ###
##### A strong, reliable multiplayer experience. #####

The goal of this project was to expand what we did in [PongBreak]() and use temporal delay and physics simulations to create a smoother, and more intricate networked system.

##### Buttery-smooth gameplay. #####

When the game is started, the server pings both clients, and then sets the temporal delay for them to a factor of the larger of the two pings, and this is then applied to both the other player and the asteroids on each client. This allows for (almost) perfectly concurrent gameplay between the clients regardless of difference in ping.

##### Physics #####

We used Box2D for our physics engine, and we implemented bullet-ship collision, bullet-asteroid collision, ship-asteroid collision, and asteroid-asteroid collision. To make sure the clients have consistent gameplay, the server is authoritative with regards to the physics calculations and velocity changes from the players, and then sends the updated positions and orientations to the clients.

* * *

### Gameplay ###
##### Asteroids, with physics. #####

Being up-front and honest, the gameplay is not as refined as PongBreak, but the functionality is all there, including screen-wrapping asteroids with little indicators, a scoring system, and round numbers (as well as lives and asteroid destruction).

##### Controls: #####
W/UP to thrust forward. A/LEFT and D/RIGHT to steer. SPACE to shoot.