# Overview of PA2: Bridger canyon  one-way simulation
### Kade Pitsch

My approach was all over the place at first but i think i got it condensed into soomething that works.
My thought process was to set up a conditional loop and i wanted to do a ```while(!condtion){ do something }``` but after several failures i could not get the logic to work with how my brain was working.  
I went with ```if(true) {do this} else {thread wait for condition}``` logic structure which ended up working for how i had everything set up. So the way i sent up the threads is that once one came into the ```onevehicle()``` function it set a "travelLock" as i called it. If a car comes in from bridger first it is going to serve all the bridger cars first and then Bozeman and vice-versa. After all the bridger cars get handled the thread cond signals and "dumps" in the cars that were waiting to go previously.