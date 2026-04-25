# Part 0, Step 2

Handling of I/O on MD407. Enter integers and returns sum and median of the last 3 integers.

## Controls

* **Start:** Enter `go` to begin.
* **Input:** Enter your desired digits. To complete the number, enter the delimiter `e`.
* **Reset:** Enter `f` to clear the history.
* **Volume Control:** Enter `q` to increase, and `a` to decrease. You can also write a number and then press `z` to increase / decrease the volume to the chosen value.
* **Mute:** Toggle mute by entering `t`.
* **Key** Writing a number and then pressing `x` increases/decreases the key to the chosen value.
* **Tempo:** Writing a number and then pressing `c` increases the tempo to the chosen value.
* **Playback Control** Pressing `p` or pressing ` `, aka space, toggles the playback and the music playing.
* **Stopping Music** To stop the music and start again from the start press `s`.

* **Claim Conductorship** Pressing `y` will try to claim the conductorship from the network.
* **Debug Printing** Pressing `d` will toggle the debug printing. This will print the tempo if the board is in conductor mode or print `MUTED` if the board is a musician that is muted.
* **Reset** Pressing `r` will reset the tempo and key IF the board is in conductor mode.

* **Failure 1** Pressing `j` will toggle the board's failure state 1, F1. When the board is in failure state, it will ignore any incoming CAN messages and will not send out any heartbeats.
* **Failure 2** Pressing `k` will start the board's failure state 2, F2. The board will leave the failure state 2 after a random delay between 5 and 10 seconds. When the board is in failure state, it will ignore any incoming CAN message and will not send out any heartbeats.
