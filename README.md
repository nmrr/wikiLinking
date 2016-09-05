# wikiLinking

## **English**

wikiLinking is used to create tree diagram of links from a Wiki page. Each Wiki page has links which point to other Wiki pages. This software retrieves these links to create a tree and can crawl to the depth 4 (4 pages after the root).

### Requirement  

To build the sofrware, it's necessary to install  a development environment and cURL library.

On Debian-based systems : 
**apt install build-essential**
**apt install libcurl4-openssl-dev**

### Compilation

**g++ -Wall -pipe -Ofast -std=c++11 main.cpp query.cpp timer.cpp -o wikiLinking -lpthread -lcurl**

### How does it work ?

**Required arguments :** 
-w (--wiki) : URL of the wiki (ex : https://en.wikipedia.org/wiki/)<br/>
-p (--page) : Root page (ex : Paris)<br/>
-r (--resolution) : Resolution domain string, page:port:IP (ex : en.wikipedia.org:443:91.198.174.192)<br/>
-f (--file) : Output file, format is CSV (ex : paris.csv)<br/>

**Optional arguments :** 
-d (--depth) : Maximum depth of the tree (1-4), default is 1<br/>
-t (--thread) : Number of threads (1-100), default is 5, please don't overload the target server !<br/>
-u (--useragent) : User agent used by the crawler<br/>
-s (--separator) : Field separator, default is ' ; '<br/>

To crawl Paris' page to the depth 4 with 10 threads : 
<br/>**./wikiLinking -w "https://en.wikipedia.org/wiki/" -p "Paris" -r "en.wikipedia.org:443:91.198.174.192" -f Paris_EN.csv -d 4 -t 10**

#### Samples

Some samples are available here : https://github.com/nmrr/wikiLinking---samples

## **Français**

wikiLinking permet de créer un arbre de liens à partir d'une page Wiki. Chaque page Wiki dispose de liens pointant vers d'autres pages. Ce logiciel permet de récupérer ces liens et peut voyager sur les Wiki jusqu'au niveau 4 (4 pages après la page racine). 

### Exigences  

Pour compiler ce logiciel, il est nécessaire d'installer un environnement de développement ainsi que la bibliothèque cURL.

Sur les systèmes basés sur Debian : 
**apt install build-essential**
**apt install libcurl4-openssl-dev**

### Compilation

**g++ -Wall -pipe -Ofast -std=c++11 main.cpp query.cpp timer.cpp -o wikiLinking -lpthread -lcurl**

### Comment ça fonctionne ?

**Required arguments :** 
-w (--wiki) : URL of the wiki (ex : https://en.wikipedia.org/wiki/)<br/>
-p (--page) : Root page (ex : Paris)<br/>
-r (--resolution) : Resolution domain string, page:port:IP (ex : en.wikipedia.org:443:91.198.174.192)<br/>
-f (--file) : Output file, format is CSV (ex : paris.csv)<br/>

**Optional arguments :** 
-d (--depth) : Maximum depth of the tree (1-4), default is 1<br/>
-t (--thread) : Number of threads (1-100), default is 5, please don't overload the target server !<br/>
-u (--useragent) : User agent used by the crawler<br/>
-s (--separator) : Field separator, default is ' ; '<br/>

Pour récupérer les liens de la page Paris jusqu'à la profondeur de 4 avec 10 threads  : 
<br/>**./wikiLinking -w "https://fr.wikipedia.org/wiki/" -p "Paris" -r "fr.wikipedia.org:443:91.198.174.192" -f Paris_FR.csv -d 4 -t 10**

### Exemples

Quelques exemples sont disponibles ici : https://github.com/nmrr/wikiLinking---samples
