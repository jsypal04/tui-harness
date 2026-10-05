# Jev Harness

This is a harness to allow jev to control my TUI applications.

This is the general architecture that I have in mind:

**TUI app:**
- Broadcasts its state in json over a unix socket
- This should simply be a representation of the UI state not any internals.
It should pretty much be all and only the information that an actual user
would see
- Send a new state whenever the state changes

**Harness**:
- Listen for state updates on the unix socket.
- Parse the state, make it into something compatible with jev
- Get jev to decided when the next action should be
    - Each action should map to a key stroke to be sent to the terminal app's stdin.
- Send the key stroke to the terminal app's stdin.
