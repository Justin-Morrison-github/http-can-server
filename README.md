# CAN HTTP Server From Scratch Using C 

## Project Idea

I want a C program to host a simple HTTP written from scratch that exposes API endpoints for nodes connected on a CAN network. I also don't want AI to write any code, I may use it for research, but all code should be hand typed. I am going to start with raw CAN for learning, but I may transition to a protocol such as CANOpen once mature.

## Endpoints

For simplicity currently I am only going to use GET HTTP requests.

### GET
- /api/health:          Return server status
- /api/stats:           # messages and nodes, data rates, and other counting/windowed stats
- /api/v1/nodes:        List of all nodes connected on the CAN bus
- /api/v1/nodes/<ID>:   Info on a single node on the CAN bus based on given ID (may use string names instaed of ID numbers directly)

## CAN

I am still debating how the CAN nodes will report their status. Currently I am imagining:
- An initilization/subscribe message that gets sent at startup 
- Some periodic Heartbeat
- Possibly periodic telemetry or on-request telemetry messages
