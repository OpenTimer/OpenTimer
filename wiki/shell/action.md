# Action Command

An action command is an operation that triggers the timing update
from the earliest task to the one that produces the result of the action call.
The time complexity of an action command is algorithm dependent.

| Command | Description |
| :------ | :---------- |
| [update_timing](#update_timing) | updates the timer to keep all timing values up-to-date |
| [report_timing](#report_timing) | reports the critical paths in the design |
| [report_at](#report_at)         | reports the arrival time at a pin |
| [report_slew](#report_slew)     | reports the transition time at a pin |
| [report_rat](#report_rat)       | reports the required arrival time at a pin |
| [report_slack](#report_slack)   | reports the slack at a pin |
| [report_tns](#report_tns)       | reports the total negative slack of the design |
| [report_wns](#report_wns)       | reports the worst negative slack of the design |
| [report_fep](#report_fep)       | reports the total failing endpoints in the design |
| [report_area](#report_area)     | reports the aggregate cell areas of the design |
| [report_leakage_power](#report_leakage_power) | reports the aggregate cell leakage power of the design |


---

## update_timing

The `update_timing` command updates the timer to keep all timing information
(slew, delay, slack) up-to-date.
This is the bottom-most call of every action command.

---

## report_timing

The `report_timing` command reports the timing critical paths in the design.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -num_paths N | optional  | the number of paths to report |

By default, OpenTimer reports the most critical path (N=1) if `-num_paths N` is not given.

---

## report_at

The `report_at` command reports the arrival time of a pin at a given 
min/max split and rise/fall transition.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -pin name | required  | the name of a pin to query |
| -min      | optional  | the minimum arrival time |
| -max      | optional  | the maximum arrival time |
| -rise     | optional  | the arrival time at the rising edge |
| -fall     | optional  | the arrival time at the falling edge |

By default, OpenTimer assumes *min* split and *rise* transition. 

---

## report_slew

The `report_slew` command reports the transition time of a pin at a given 
min/max split and rise/fall transition.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -pin name | required  | the name of a pin to query |
| -min      | optional  | the minimum transition time |
| -max      | optional  | the maximum transition time |
| -rise     | optional  | the transition time at the rising edge |
| -fall     | optional  | the transition time at the falling edge |

By default, OpenTimer assumes *min* split and *rise* transition. 

---

## report_rat

The `report_rat` command reports the required arrival time of a pin at a given 
min/max split and rise/fall transition.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -pin name | required  | the name of a pin to query |
| -min      | optional  | the minimum required arrival time time |
| -max      | optional  | the maximum required arrival time time |
| -rise     | optional  | the required arrival time time at the rising edge |
| -fall     | optional  | the required arrival time time at the falling edge |

By default, OpenTimer assumes *min* split and *rise* transition. 

---

## report_slack

The `report_slack` command reports the slack of a pin at a given 
min/max split and rise/fall transition.

| Argument | Type | Description |
| :------- | :--- | :---------- |
| -pin name | required  | the name of a pin to query |
| -min      | optional  | the minimum slack time |
| -max      | optional  | the maximum slack time |
| -rise     | optional  | the slack time at the rising edge |
| -fall     | optional  | the slack time at the falling edge |

By default, OpenTimer assumes *min* split and *rise* transition. 

---

## report_tns

The `report_tns` command reports the total negative slack 
(the sum of negative slack across all endpoints)
in the design.

## report_wns

The `report_wns` command reports the worst negative slack in the design.

## report_fep

The `report_fep` command reports the total failing endpoints in the design.

## report_area

The `report_area` command reports the aggregate cell areas in the design.

## report_leakage_power

The `report_leakage_power` command reports the aggregate cell leakage power in the design.

* * *

[Cpp-Taskflow]:                 https://github.com/cpp-taskflow/cpp-taskflow/





