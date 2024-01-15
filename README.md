# SOBay---UNIX-Auction-Platform
Development of an auction management platform called "SOBay" in C for UNIX systems (Linux). The project was undertaken as part of the curriculum for the Bachelor's degree in Computer Engineering at ISEC, specifically in the Operating Systems course. The focus is on using system calls instead of library functions.

# User Interface (Frontend)
Interaction via command line for clients. Commands include placing items for auction, listing items, checking balance, making bids, etc.

# System Operation (Backend):

<li>Interaction with administrators, frontends, and promoters simultaneously.</li>
<li>Detects when a client exits without notice and notifies the administrator.</li>
<li>Manages promoters, triggering promotions based on their outputs.</li>
<li>Handles the orderly shutdown of all components.</li>

# Requirements and Constraints:

<li>Implementation must follow the Unix system mechanisms discussed in the course.</li>
<li>Named pipes are used for communication between frontend and backend.</li>
<li>Only libraries provided by the instructors are allowed.</li>
<li>Synchronization issues must be handled appropriately.</li>
