// The entry file of your WebAssembly module.
import { Console, Environ, Date } from "as-wasi";
export function _start(): void {
  let env = new Environ();

  let msg = env.get("HOME");

  if (msg) {
    Console.log(msg);
  } else {
    Console.log("Idling...");
  }

  while (true) {}
}
