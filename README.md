# Maintenance reminder for [Carloop](https://carloop.io)

*Get a text message from your car every 5000 miles!*

Publishes an event to the Particle cloud every time a mileage interval is reached.

Gets the mileage by continuously reading the vehicle speed through the
OBD port and integrating it to get the mileage. There is unfortunately
no standard way to get the odometer miles through OBD.

## [Flash it to your Carloop and use it right away](https://www.carloop.io/apps/app-reminder)

## Building

Copy this application to Particle Build and add the [Carloop library](https://build.particle.io/libs/56eebf35e1b20225ce00048d)

## Usage

On first use, a 5000 miles interval is started.

The interval can be changed the [web UI](https://www.carloop.io/apps/app-reminder) or by calling functions through the [Particle CLI](https://www.particle.io/cli).

## Remote Functions

| Name | Argument | Description |
|------|----------|-------------|
| reset | none | Reset the mileage interval counter to 0 mi |
| limit | `value` | Change the upper bound of the mileage interval to the value in miles |

Call one of the functions with the CLI using `particle call my_carloop limit 7500`

## Remote Variables

| Name | Description |
|------|-------------|
| count | Current value of the mileage interval counter |

Get the value of one of these variables with the CLI using `particle get my_carloop count`

## License

Copyright 2016 1000 Tools, Inc.

Distributed under the MIT license. See [LICENSE](/LICENSE) for details.

