# Barber

This repository implements a deadlock-free solution to the famous Synchronization problem called the Sleeping Barber.

## Definitons

`num_customer`: The number of customer that comes to the barber within given time.

`max_arrival_time`: The maximum time span between the customers arriving to the barber.

`max_haircut_duration`: The maximum time duration of a single haircut.

`haircut_repetition`: The maximum haircut repetition number that a customer will have.

### Input Format

    ./barber <num_customer <max_arrival_time> <max_haircut_duration> <haircut_repetition>

## Execution

Run the following command to compile the C code

    make

Then you can enter your input, e.g.,

    ./barber 25 500 100 50
