# Perennial Garden

The precursor of this game was conceived and created during Global Game Jam 2021. It is more simulation than game. 

This game runs on a series of Blinks, a game platform produced by [Move38](https://github.com/Move38). To install it, you need blank Blink to load it onto. To learn more about the Blinks game platform, click [here](https://move38.com/). If you do not own any Blinks, you can run this game using the Blinks online simulator [here](https://move38.github.io/Blinks-Simulator/).

At the beginning of the game, all tiles are yellowish or white. These tiles fade and grow brighter on a four-second loop to simulate the sun rising and setting. They grow whiter and yellower on a variable 7.8-to-16 second loop to simulate a summer to winter season. 

Click anywhere to add water to a tile. That tile will become green with a blue dot on one pip to denote that it is a water tile. The blue dot will become lighter and darker on a variable 5.6-to-8.0 second loop to simulate a water table that ebbs and flows on a monthly cycle. When the water level is high enough, the tile will begin to grow flowers during the day, even sometimes in winter. The flowers will fade or close again as the sun sets. 

The water tile will also share water with its neighbors, but these neighbors will not get blue pips. If the neighbors have enough water and sunlight, they will begin growing flowers, too. 

You can water more than one tile. For best results, use a greater numbers of blinks and water at least two tiles near an edge or corner. See images in this folder for example tableaus with 18 and 30 blinks. This simulation can run with as few as one blink, but it is less interesting at lower numbers.

If you water too many tiles, click a water tile again to turn off the water. That tile's water will recede over time, along with the water in the surrounding tiles.

The game does not require tile movement, but moving a water tile slowly around the garden could cause interesting effects.

# Acknowledgements

Thanks to members of the Blinks Discord for their encouragement, advice, and assistance with debugging and learning the code, especially [Guardian-of-fun](https://github.com/Guardian-of-fun) and [ReluctantPirate](https://github.com/ReluctantPirate). Thanks to [RunDotGames](https://github.com/RunDotGames) for suggesting a way to turn off water.

