<h1 align="center">
    🐙 OctopusOS 🐙
</h1>

---

OctopusOS: if you got tired of your boring windows and want something more fun and with more limbs try OctopusOs, OctopusOs is a multi process scheduler and memory manger simulator made for Linux.

<video width="620" height="320" controls>
  <source src="./public/vids/req1.mp4" type="video/mp4">
</video>

---

## 👾 Features:

- **RR** (Round Robin) Scheduling
- **SJF** (Shortest Job First) Scheduling
- **HPF** (Highest Priority First) Scheduling
- Doubly list, Priority Queue, Queue, and binary tree data structures implementation.
- Memory management with **buddy system** algorithm.
- Multi process scheduler process clock generator for maximum use of **IPC** (Inter Process Communication).
- **GUI** built with C and raylib/raygui library.
- Expressive output featuring:
  - log of all process info in scheduler.log
  - log of all memory info in memory.log
  - performance metrics in scheduler.perf
  - Fancy Terminal output and **ascii art**.
  - Images exported of the system each run include the scheduler log and perf log.

> [!NOTE]
> GUI is bonus feature you can do it with any language you want Ex: `JS(Electron)`

![OctopusOS](./public/imgs/octo-assci.png)

## 🧰 Tech Stack:

- C lang
- `raylib` for GUI.

## 🚀 Quick Start:

- Clone the repository
- ensure you have make and `raylib` installed.
- Run make in `code` folder.

```bash
make
```

<!-- <h1 align="center">
    <img src = "./public/imgs/Octopus.jpg">
    <br/>
</h1> -->

> [!CAUTION]
> In memory management we didn't handle not having enough space for a process.

## System Requirement

<h2 align="center">
    <img src = "./public/imgs/prcoess-scheduler.excalidraw.png">
    <br/>
</h2>
