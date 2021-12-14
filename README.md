
# Instructions

*The "Full Release" folder contains the exec and all its dependencies.*

1. Locate your host/server's ip address. You'll need it when running the client.

1.1 cmd -> ipconfig.

2. **FIRST** Launch the application ( *run bbKvm-vc.exe* ) on the server device , and use the 'server' command.

> ***WARNING*** once the server is waiting for a client, closing the program directly (x'ing out the window) may cause memory leaks. You can simply open a second instance of the program as a client on the same device using 'localhost'. This will allow you to exit out of the program safely.

3. **THEN** Launch the application on your client device. Type your host device's ip right after the client command.

  

| KEYBIND | Function

|----------------|-------------------------------|

|TAB  |`Switch between controlling host device and remote device.`

|ESC |`Shuts down program.` |

  

# Barebones KVM

>💡 Inspiration

  

You may have heard about KVM (*Keyboard, Video, Mouse*) switches that allow you to use one set of peripherals to control multiple computers. There are virtual KVM programs like [Synergy](https://symless.com/synergy) and [ShareMouse](https://www.sharemouse.com/) that achieve very similar functionality without the need for any extra hardware.

  

Unfortunately, when trying to research how programs like these may work I couldn't seem to find anything that answered my questions. And so, I decided to build a program that achieved similar functionality in order to get an idea on how it all works.

  

Luckily, I was able to find [Barrier](https://github.com/debauchee/barrier), an open-source virtual KVM repo that I was able to reference/use as practice for reading large codebases I was not involved in creating.

  
  

## ❓ How it works

Whilst researching and planning, I broke down the project into 4 sub-problems:

1. How to ***communicate*** between two computers.

2. How to ***intercept*** keyboard and mouse input.

3. How to ***send*** the input over the network.

4. How to ***simulate*** the input in the client.

  

### 🌐 Networking

The toughest part of figuring out the networking, was **understanding** what was going on. Video tutorials like the ones by [Sloan Kelly](https://www.youtube.com/watch?v=WDn-htpBlnU&list=PLZo2FfoMkJeEogzRXEJeTb3xpA2RAzwCZ), and the [Windows docs](https://docs.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock) made it really easy to get simple connections running that I could then use to learn.

  

Before moving forward, I wanted to *really* understand what the code was doing and why. To do so, I kept asking myself questions like what even ***is*** a socket? What is a file descriptor? How does a socket work? How do the client and server find one another? What allows a server to handle multiple connections? etc.

  

The actual networking code remains pretty boilerplate. The server binds a socket to all available ip addresses and port `27015` and waits for a client to connect.

  

### ✉ Hooks & Message Queues

Now I needed to answer the big question, *How can my program listen to keyboard and mouse input?* Fully understanding how to achieve this required learning about [Keyboard Input](https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input), [Mouse Input](https://docs.microsoft.com/en-us/windows/win32/inputdev/about-mouse-input), [Hooks](https://docs.microsoft.com/en-us/windows/win32/winmsg/about-hooks), and [Message Queues](https://docs.microsoft.com/en-us/windows/win32/winmsg/about-messages-and-message-queues).

  

Windows functions through messages that are passed through processes. Message pumping allows you to intercept this messaging system.

  

Hooks allow your process to attach itself to specific message queues as they are processed through your system. The keyboard (*WH_KEYBOARD_LL*) and mouse (*WH_MOUSE_LL*) hooks are the essence of our program. As Windows passes all captured input to our process, we could then analyze it and do with it as we wanted. *Ignore it, break it down, prevent it from being processed by the rest of the system, etc.*

  

Once the intercepted message is broken down into something our program can communicate over the network, a message is inserted into the Message Queue that can then be processed in our main network loop. In the main loop, the data is serialized into a string, as the send() function takes in a char array. The first element of our string will always be the message type, so that our client knows how to interpret the message. It is then sent to the client.

  

### ⌨🖱 Simulation

All that is left is analyzing the data sent over the network at the client, and using it to simulate the input sent by the server. Thankfully, Windows provides us with the [SendInput](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendinput) function, which we can use for both keyboard and mouse input simulation.