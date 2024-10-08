# C++ with WebUI

This repository **demonstrates how to create a C++ backend** software working together with a **Javascript/Typescript/Svelte frontend**.

This is a very simple and ad-hoc solution without any claim to completeness regarding the possibilities to communicate or other possible solutions to the problem.

This repository does not discuss possibilities to encode/decode messages (marshall/unmarshall, serialize/deserialize). Instead of using some sophisticated data representation (like JSON or [MessagePack](https://msgpack.org/)) and a schema (like [JSON Schema](https://json-schema.org/)) together with some code generation (like [Quicktype](https://quicktype.io/)), we just transfer raw strings or binary blobs in this example.

## Use Case (what is this good for?)

When implementing a network-only C++ data processing software (data-in, data-out), you can provide an easy to use command API with a nice UI to control you software and to interact with it.

You could also do some number crunching in C++ and provide a network API for a software written in another language (like Typescript, Java, C# or Python) implementing the computationally less critical logic.

## How to run/explore

* You need node 20 for the frontend and a C++ compiler with cmake.
  * Easiest way to get the dependencies: use the devcontainer (vscode plugin `ms-vscode-remote.remote-containers`; you will need Dockers enabled on your system!)
  * Open project in the devcontainer and proceed with the next steps (all dependencies are included in the [Dockerfile](.devcontainer/Dockerfile))
* Run `./build-and-run.sh`, it
  * compiles the webui (node, Typescript)
  * compiles the backend (cmake, C++)
  * runs the backend (which serves the webui and offers a simple API)
* Once the backend runs,
  * open the indicated URL [`http://0.0.0.0:18080`](http://0.0.0.0:18080).
  * Note: inside a docker you will need to use a forwarded address: ![image](./doc/images/docker.png)
* The browser shows an page where
  * you can enter a number an trigger an action on C++ (the result is send back to the browser and is displayed).
  * you should see some logs ("some extra logs from C++ (number)" with increasing numbers).
  
  ![webui](./doc/images/webui.png)

**WebUI**: when pressing the button, the WebUI (Typescript) requests some C++ computation (`compute1/<number>`):

```ts
  const data = await fetch(
      `http://${host}:${port}/compute1/${someNumber}`
    )
    .then((x) => x.json());
```

**Backend**: the `fetch` command above triggers the following C++ code:

```cpp
  CROW_ROUTE(app, "/compute1/<int>")
  ([](int x) {
    CROW_LOG_INFO << "...returning 2 x " << x;
    crow::json::wvalue res({{"result", 2 * x}});
    return res;
  });
```

**Python "tester"**: the C++ code can also be triggered via other languages (here: Python; can be executed in the docker via `python python/python_test.py`):
```python

num=123;
res = requests.get(f"http://localhost:18080/compute1/{num}")
response = json.loads(res.text)

assert response["result"] == num*2
```

## Details

The manual **frontend code (WebUI) is about 120 LOC** (including HTML and CSS via Svelte). The **C++ code about 90 LOC**.

### C++ Backend

* Dependencies: C++20, Crow
* main entry point: [main.cpp](./backend/main.cpp)
  * serves the UI (see: [CMakeLists.txt](./backend/CMakeLists.txt), where `CROW_STATIC_DIRECTORY` points to the folder, where the WebUI is compiled to)
  * offers the URL `/compute1/<int>` to trigger a HTTP-GET command (illustrated in the section **How to run/explore**).
  * offers a WebSocket at `/logs` to provide the crow logs to the WebUI

### WebUI

Note: to get full vscode support while editing you should open this folder in an own vscode instance. You can `npm run dev` and connect to a separately running C++ backend. You can also connect multiple times to the C++ server.

* Dependencies: Typescript, Svelte 5
* main entry point: [Simple.svelte](./ui/src/lib/Simple.svelte)
  * renders a UI
  * allows to enter a number, allow to press a button to send a command (HTTP-GET; illustrated in the section **How to run/explore**) to C++ and display the answer from C++.
  * connects to the WebSocket server from C++ and shows all logs.
