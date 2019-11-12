# Best team

In this project, given a list of Fanstasy Football players, we have to compute the best football team with a certain formation, i.e. the one with the highest amount of points, such that its cost does not exceed a given budget.

# Our solution

We have realized that is problem is really similar to the classical knapsack but with certain restrictions:

In the knapsack problem we are given a knapsack with weight storage capacity W, and n objects, each with a certain value v, and a certain weight w. With this information we are asked to find a selection of objects that maximizes the sum of values and such that the sum of weights is below the knapsack threshold.

In our case the capacity of the knapsack would be our budget and the objects would be the players, where the weight is the price of a player and its value is the amount of points it has.

However there are two extra conditions.
* The price of a player can't exceed a certain amount. This can be solved just considering players whose price does not exceed the threshold
* We have to form a fixed formation (1 goalkeeper, N1 defenses, N2 midfielders, N3 forwards)

To deal with the formations we can modified the states of the dynamic programming used in the knapsack problem.

If we could take any amount of players and of any position the states of the dynamic would be the index of the player that we are considering (the index they have in a vector where they are stored) and the budget we still have.

That is, dp\[k\]\[w\] = maximum points that can be obtain with a budget of w and considering only those players whose index is at least k.

If we take into account the formation then we could just consider:

dp\[k\]\[w\]\[n0\]\[n1\]\[n2\]\[n3\] = maximum points that can be obtain with a budget of w, considering only those players whose index is at least k, taking exactly n0 goalkeepers, n1 defenses, n2 midfielders and n3 forwards.


# Personal choices

* We have decided to ...
