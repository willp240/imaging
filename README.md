# imaging
Code for attempts at imaging events in SNO+

To setup, first clone this repository

Then edit env.sh to point to your specific directories. $IMAGING_ROOT is the directory you've just cloned into. $DATA_ROOT is where you want things to get written to (subdirectories below this will be created if using the condor batch script supplied).
You can also specify another environment file to get called when submitting jobs to batch systems. This should set your ROOT and everything else needed for RAT. Then you also specify your $RATROOT. $RATROOT/env.sh gets sourced when using the submission script.

Then, to build, simply run:

make


Currently there are several apps which use various implementations of the adaptive grid method to search over possible emission times/positions. These are still being frequently updated and refined. More documentation will be written when these are more settled. To run, do something like:

> ./bin/adaptive_grid_tsel /path/to/input/file /path/to/output/file

There are a few scripts setup for submitting to the Oxford batch machines with Condor. Run with:

> python /submission/submitNoArgs.py adaptive_grid_tsel /path/to/input/file jobname

The ouput file will be written to $DATA_ROOT/jobname/jobname.root. Logs, output, errors, and the submission files will be written to $DATA_ROOT/jobname/log/jobname.log, $DATA_ROOT/jobname/output/jobname.output, $DATA_ROOT/jobname/error/jobname.error, $DATA_ROOT/jobname/submit/jobname.submit and $DATA_ROOT/jobname/sh/jobname.sh.

The following is outdated and will updated
imaging.cc:
The only app currently written is imaging.cc. This is still slightly hardcoded for specific file with a muon starting at (0,0,8500)mm going in the (0,0,-1) direction. This will be generalised in the future.

The code first gets the event from the inputted file, the PMT info and the fitted position/time.

A "MegaCube" is set up to scroll through 11 positions in x y and z. They are spaced 1000 mm apart in x and y, and 50 mm apart in z. They are centred around the fit position. 

These xyz positions are scrolled through for 30 different initial times, 1 ns apart, centred on the fit time.

For each initial time, the 3D grid of positions is scrolled through. For each hit, for each position, the time residual is calculated assuming the particle travelled from the fit position, to this position, and then emitted a photon. If the time residual is within 0.5 ns of 0 ns, the TH3D representing the cube for that initial time is filled at that position.

All TH3DS are written to file.

The true and fitted times and positions are printed out initially. The true times are what is hardcoded here, as these are calculated as the time and position of the muon when it reaches the same Z as the fit position. The actual imaging doesn't use this so doesn't depend on any hard coding for a specific file.

imaging_alg2.cc:

This is similar to imaging, but rather than use the tof of muon+photon (which assumes straight path of the muon) we fit emission time at each position.

mcmc.cc:

Run an MCMC instead of grid search over times and positions.

mcmc_fixedT.cc:

Run an MCMC instead of grid search over positions for a fixed time. Submit to condor using submitFixedT.py. 

Plotting Scripts:
There is a plotting script, PlotTH3s.cpp, with two functions.

"PlotAll" takes in the output filename from running imaging.cc, and plots all 30 TH3Ds to a canvas. It also prints out the time offset of the TH3D with the highest integral.
Run as:

> root -l
> 
> .L PlotTH3s.cpp+
> 
> PlotAll("outputtedFileName.root")

"Plot1" takes in the output filename from running imaging.cc, and the time offset for the histogram you want to draw.
Run as:

> root -l
> 
> .L PlotTH3s.cpp+
> 
> Plot1("outputtedFileName.root", timeOffset)

"BestTandLLH.cpp" is intended for use with the output of imaging_alg2.cc. It makes two TH3Ds, containing the best LLH and corresponding emission time for each position.

"ConvertHist.cpp" was used for converting between histograms with each bin filled, and having only every tenth bin filled (to try and see in 3d better)

"PlotFromTree.cpp" and "PlotMultTrees.cpp" are used for plotting from the output of the mcmc apps.

"plotsph.py" was for testing drawing the AV on top of tracks

Classes:
"Cube" contains the centre coordinates of the cube your testing, it's "radius", best fit time, best fit overlap, and a method to divide it into a collection of sub cubes.
"CubeCollection" is essentially a vector of cubes
