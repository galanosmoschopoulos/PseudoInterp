<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a name="readme-top"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]



<!-- PROJECT LOGO -->
<br />
<div align="center">
<h3 align="center">PseudoInterp</h3>

  <p align="center">
    An interpreter of IB pseudocode built from scratch in C++!
    <br />
    <a href="https://github.com/rafmosch/PseudoInterp">View Demo</a>
    ·
    <a href="https://github.com/rafmosch/PseudoInterp/issues">Report Bug</a>
    ·
    <a href="https://github.com/rafmosch/PseudoInterp/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

<!-- [![Product Name Screen Shot][product-screenshot]](https://example.com) -->

This project is an interpreter of the IB pseudocode language built from scratch in C++. IB pseudocode is used in the IB CS course to write simple algorithms and programs. It contains all basic procedural structures (while, for, if/else), functions, primitive types (bool, float, int) arrays and strings, and ADTs (stack, queue, linked list/collections). It is a pseudo-OOP language, as ADTs are treated as objects but the user cannot define their own objects.

The interpreter utilizes a recursive descent parser to build the abstract syntax tree, which implements runtime polymorphism to represent all structures/expressions as nodes. Types are runtime-resolved and objects utilize sum-type std::variant to store the appropriate type. Some built in functions such as constructors for ADTs and input/output are hardcoded.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Built With
* [![C++][Cpp-badge]][Cpp-url]

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- GETTING STARTED -->
## Getting Started

This is a guide of how to build the interpreter on your machine and run an example program.

### Prerequisites

This is an example of how to list things you need to use the software and how to install them.
* g++ 11.3.0
* GNU make 4.3
* git 2.34.1

On Ubuntu:
  ```
  $ sudo apt install git build-essential
  ```

### Compilation

1. Clone the repo on your machine.
  ```
  $ git clone https://github.com/rafmosch/PseudoInterp
  $ cd PseudoInterp/
  ```
2. Build
  ```
  $ make clean
  $ make
  ```
  The binary can be found in the `/bin` folder.

### Running a program

The following flags are available:
* -?: Prints informational message regarding usage.
* -v: Prints program version.
* -i: Sets input file.

To execute the example program, run `$ ./bin/pseudointerp -i ./examples/ex1`.
This program reverses the contents of a stack.

More information on syntax can be found by referring to the official IB pseudocode guide, and looking at more examples in `/examples`.


<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- USAGE EXAMPLES -->
## Usage

* Test pseudocode programs, or practice for IB tests/exams.
* Quickly implement various algorithms, as all basic ADTs are available.

<!--_For more examples, please refer to the [Documentation](https://example.com)_-->

<p align="right">(<a href="#readme-top">back to top</a>)</p>


See the [open issues](https://github.com/rafmosch/PseudoInterp/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Your Name - [@raf_mos](https://twitter.com/raf_mos) - raf.mos@princeton.edu

Project Link: [https://github.com/rafmosch/PseudoInterp](https://github.com/rafmosch/PseudoInterp)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* []()
* []()
* []()

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/rafmosch/PseudoInterp.svg?style=for-the-badge
[contributors-url]: https://github.com/rafmosch/PseudoInterp/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/rafmosch/PseudoInterp.svg?style=for-the-badge
[forks-url]: https://github.com/rafmosch/PseudoInterp/network/members
[stars-shield]: https://img.shields.io/github/stars/rafmosch/PseudoInterp.svg?style=for-the-badge
[stars-url]: https://github.com/rafmosch/PseudoInterp/stargazers
[issues-shield]: https://img.shields.io/github/issues/rafmosch/PseudoInterp.svg?style=for-the-badge
[issues-url]: https://github.com/rafmosch/PseudoInterp/issues
[license-shield]: https://img.shields.io/github/license/rafmosch/PseudoInterp.svg?style=for-the-badge
[license-url]: https://github.com/rafmosch/PseudoInterp/blob/master/LICENSE
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/rafael-moschopoulos-50161b281/
[product-screenshot]: images/screenshot.png

[Cpp-badge]: https://img.shields.io/badge/C++-blue?style=for-the-badge&logo=cplusplus&logoColor=ffffff
[Cpp-url]: https://isocpp.org/




