## Introuction
PatchMatch is the state-of-the-art stereo match algorithm framework.

Source code for the paper[1] and it's variant is implemented.

Some experiments results are listed follwing.


## Experiment of different propagation way
This experiment explore the the affects of different propagation way of PatchMatch.




+ raw(the origin propgation way of PatchMatch)
+ rows cols sweep(rows sweep, cols sweep, reverse rows sweep, reverse cols sweep)
+ red black(split pixels to red and black, each turn traverse the red pixels or black ones respectively.

<img src="./docs/experiment1_left.png"  width="400" /><img src="./docs/experiment1_right.png"  width="400" />


Left and right image show the affects of different propagation way of the left and right disparity respectively. The horizontal axis is the number of iterations, and the vertical axis is bad error 1.0 rate

We can see, raw propagation way is the best(red line), 2 iterations converge; Rows cols sweep propagation way is the second(green line), 3 iterations converge; Red black propagation way is the worse(blue line), also 3 iterations converge, but worse bad error 1.0 rate. 







## Experiment of different steps of PatchMatch
	
This experiment explore the the affects of different steps of PatchMatch.
Left image is left disparity map comparison and right image is right disparity map comparison.


<img src="./docs/experiment2_fig1.png"  width="400" /><img src="./docs/experiment2_fig2.png"  width="400" />

The images above show the PatchMatch has plane refinement or not, obviously, plane refinement has sufficient effect, reduce about 20%-30% bad error 1.0.

<img src="./docs/experiment2_fig3.png"  width="400" /><img src="./docs/experiment2_fig4.png"  width="400" />

The images above show the PatchMatch has view propagation or not, obviously, view propagation has limited effect for raw PatchMatch, but for it's variants, has some effect in the first few iterations, but useless after 3 or 4 iterations for rwos cols sweep and 6 or 7 iterations for red black.


<img src="./docs/experiment2_fig5.png"  width="400" /><img src="./docs/experiment2_fig6.png"  width="400" />

The images above show the PatchMatch has post-processing or not, obviously, post-processing improve the result a lot, reduce about 10% bad error 1.0.



## Experiment of different window size


## Conclusions
In conclusion, the the propagation way of raw PatchMatch method achieve the best result in all experiments. But, the raw propagation way is sequential implementation, Unable to parallel. And it's variants can be implemented parallel but with slowly bad error 1.0 rate increase, so it some trade off. More analysis can refers to the report.




## Citations
[1] PatchMatch

[2] Gipuma

[3] other propagation way
