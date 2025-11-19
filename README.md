# CsillagPuszta Browser
<br>
<br>

**Surf the *cyberspace* with the newest, coolest, most cosmos-worthy browser on the market - CsillagPuszta!**

CsillagPuszta (Literally translating to "star steppe" - quite a poetic name for space, don't you think?) is basically my second try at creating a web browser after my [Puszta Browser](https://github.com/Laky2k8/PusztaBrowser/) project this summer.
In a way, it can be seen as a Puszta 2.0, hence the name :)

This time I approached the topic a bit more logically: Instead of cooking up my own text renderer via Freetype and my semi-own HTTPS client, I decided to use Raylib (builtin font rendering, huzzah!) and the to be honest quite phenomenal [BearHttpsClient](https://github.com/OUIsolutions/BearHttpsClient) libraries,
which allowed me to focus on other parts of the browser I had neglected in the previous project: HTML parsing and rendering. 
As such, this browser is much more HTML-compliant and does actually support CSS (although only the builtin styling for now, I will add actual CSS parsing later!).


### But what amazing features does this browser have?
Very similar to Puszta Browser, actually! It has the standard HTTP and HTTPS support (with redirects ofcourse ;] ), but now with actual proper HTML parsing and layouting.
It also has the same stylish and modern UI ported from the original Puszta Browser.

However, the most important difference here is that this browser project should be much more flexible, modifiable and all in all "continuable" - to be honest I kind of stopped working on Puszta Browser because of how much of a sphagetti code it became.
Although this project is not perfect either I think it's much more adaptable so I might work more on this iteration; And hey, maybe I'll make an actual competent browser at the end of it ;)
<br>
<br>


# Screenshots

<img width="802" height="632" alt="screenshot_win11" src="https://github.com/user-attachments/assets/3ceb72e8-7a21-4ff6-a5d6-f805bf5deb4c" />

*Windows 11*


<img width="auto" height="632" alt="screenshot_mint" src="https://github.com/user-attachments/assets/1488204a-cd70-48bf-8d8e-ceb86bd446dd" />

*Linux Mint 21.2 "Victoria"*






## How to build
If you have G++ set up on your computer, than building the project should be as simple as just typing in `make`. If you use Windows, you should use the `Makefile` while on Linux you should use `Makefile.linux`.


## How to use

On Windows:
-  Download the newest release
-  Unzip
-  Run `csillagpuszta.exe`.

On Linux:
- Download the newest release (both the zip and `csillagpuszta` the executable)
- Unzip
- Take the assets folder from the unzipped build
- Run `csillagpuszta`. Tested on **Linux Mint 21.2 x86_64**.
