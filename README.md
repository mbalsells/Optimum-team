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

The number of states is (k+1)·(w+1)·(n0+1)·(n1+1)·(n2+1)·(n3+1). Given the restrictions of the problem we have that this number is at most 160·(k+1)·(w+1). And given that in our data set there are 505 players and the price of the 11 most expensive players is 521986875, we get that the number of states is approximately 4.22e13. So we could solve any given test within 11.7 hours, which is not that bad because we would be able to compute the exact solution within a reasonable amount of time. 

However, we can perform better. We noticed that the player prices were really high, thus resulting in having a lot of states, but this is not the case of the player points, which tend to be small, in fact the points of the 11 players with higher amount of points is 2852. So by considering the dyanamic program:

dp\[k\]\[p\]\[n0\]\[n1\]\[n2\]\[n3\] = minimum price in which we can obtain exactly p points, considering only those players whose index is at least k, taking exactly n0 goalkeepers, n1 defenses, n2 midfielders and n3 forwards.

We now have approximately 2.3e8 states, so we should be able to get the maxmium punctuation that can be obtain in less than a second, which is a great improvement.

In the real problem we also need to have for each state the actual team, which can be represented as a vector of int (with the indices of the players). the size of this vector is at most 11, meaning that our program will run within a couple of seconds.


# Personal choices

* We thought that is wasn't nice to have a vector of 6 dimensions representing the states of the dynamic and another vector of 7 dimensions representing the optimum team for a state of the dynamic. So we decided to reduce the number of states, even if this makes us use more memory. In particular we decided to reduce the four states \[n0\]\[n1\]\[n2\]\[n3\] to 2 states \[position\]\[players that we have to take of this position\]

* 
