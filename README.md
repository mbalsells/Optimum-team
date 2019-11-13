# Best team

In this project, given a list of Fanstasy Football players, we have to compute the best football team with a certain formation, i.e. the one with the highest amount of points, such that its cost does not exceed a given budget.

# Our solution

We have realized that is problem is really similar to the classical knapsack but with certain restrictions:

If we could take any amount of players and of any position the states then we would have the knapsack problem (where the prices are the weights in the classical problem and the points are the values). So we could solve the problem using dynamic programming where the states would be the index of the player that we are considering (the index they have in a vector where they are stored) and the budget we still have.

That is, dp\[k\]\[w\] = maximum points that can be obtain with a budget of w and considering only those players whose index is at least k.

However there are two extra conditions.
* The price of a player can't exceed a certain amount. This can be solved just considering players whose price does not exceed the threshold
* We have to form a fixed formation (1 goalkeeper, N1 defenses, N2 midfielders, N3 forwards)

If we take into account the formation then we could just consider:

dp\[k\]\[w\]\[n0\]\[n1\]\[n2\]\[n3\] = maximum points that can be obtain with a budget of w, considering only those players whose index is at least k, taking exactly n0 goalkeepers, n1 defenses, n2 midfielders and n3 forwards.

The number of states is (k+1)·(w+1)·(n0+1)·(n1+1)·(n2+1)·(n3+1). Given the restrictions of the problem we have that this number is at most 160·(k+1)·(w+1). And given that in our data set there are 505 players and the price of the 11 most expensive players is 521986875, we get that the number of states is approximately 4.22e13. So we could solve any given test within 11.7 hours, which is not that bad because we would be able to compute the exact solution within a reasonable amount of time. 

However, we can perform better. We noticed that the player prices were really high, thus resulting in having a lot of states, but this is not the case of the player points, which tend to be small, in fact the points of the 11 players with higher amount of points is 2852. So by considering the dyanamic program:

dp\[k\]\[p\]\[n0\]\[n1\]\[n2\]\[n3\] = minimum price in which we can obtain exactly p points, considering only those players whose index is at least k, taking exactly n0 goalkeepers, n1 defenses, n2 midfielders and n3 forwards.

We now have approximately 2.3e8 states, so we should be able to get the maxmium punctuation that can be obtain in less than a second, which is a great improvement.

In the real problem we also need to have for each state the actual team, which can be represented as a vector of int (with the indices of the players). the size of this vector is at most 11, meaning that our program will run within a couple of seconds.

... why = 
# Personal choices
* To make the code simpler we denoted each position with a number:
0 - goalkeeper, 1 - defense, 2 - midfielder, 3 - forward  

* We decided to separate the players according to their position. In our program classified_players\[pos\] is a vector with all the players which position is pos.

* We thought that is wasn't nice to have a vector of 6 dimensions representing the states of the dynamic and another vector of 7 dimensions representing the optimum team for a state of the dynamic. So we decided to reduce the number of states, even if this makes us use more memory. In particular we decided to reduce the four states \[n0\]\[n1\]\[n2\]\[n3\] to 2 states \[position\]\[players that we have to take of this position\]. So, at the end, we have that our memoization vector will be.

dp\[p\]\[pos\]\[left\]\[k\] = minimum price of a team that has exactly p points, such that it has Ni players of the i-th position with i > pos and left players of the position pos, all of which have index at least k. Here Ni denotes the number of required players that belong to the i-th position our finall team needs to have.

Whenever we change the position we are considering to the position i, we will update left to be Ni
and the recurrence is just dp\[p\]\[pos\]\[left\]\[k\] = min(dp\[p\]\[pos\]\[left\]\[k+1\], price of the player with index k with position pos +  dp\[p - points of the player with index k with position pos\]\[pos\]\[left-1\]\[k+1\])
and the optimum team will be best\[p\]\[pos\]\[left\]\[k+1\] if the first argument of the min is the minimum and best\[p - points of the player with index k with position pos\]\[pos\]\[left-1\]\[k+1\] union k if the second argument of the min is the minimum.

Realize that the finall solution will be max p such that dp\[p\]\[0\]\[1\]\[0\] <= Budget. 

* For covenience, i.e. in order to get a simpler code, we say that dp\[p\]\[pos\]\[left\]\[k\] = INF if there is no team that satisfies the constraints.

* Also for convenience when changing the position we are considering, we will save the minimum price and the team we have obtained in the "old" states. I.e. if there are K0 goalkeepers and we are in the case (p, 0, 0, K0) then we know that er are really in the case (p, 1, N1, 0), and we will work as if we are in this second case, but we will store all the information we have computed as if we were in the state (p, 0, 0, K0). We decide to store them in this "old" state, because from the previous states we will try to reach this state to get the computed team. 

* Since we will be introducing the team members indices in order (first goalkeepers, then defenses, ...) Then there is no need to store pairs of ints telling us the position and index within the position of the taken players, it sufficies with taking the indices within their position.
