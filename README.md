# wikiLinking

## **English**

wikiLinking is used to create tree diagram of links from a Wiki page. Each Wiki page has links which point to other Wiki pages. This software retrieves these links to create a tree and can crawl to the depth 4 (4 pages after the root).

#### Requirement  

To build the sofrware, it's necessary to install cURL library.
On Debian-based systems : **apt install libcurl4-openssl-dev**

#### Compilation

**g++ -Wall -pipe -Ofast -std=c++11 main.cpp query.cpp timer.cpp -o wikiLinking -lpthread -lcurl**

##### Samples

Some samples are available here : https://github.com/nmrr/wikiLinking---samples

## **Français**

wikiLinking permet de créer un arbre de liens à partir d'une page Wiki. Chaque page Wiki dispose de liens pointant vers d'autres pages. Ce logiciel permet de récupérer ces liens et peut voyager sur les Wiki jusqu'au niveau 4 (4 pages après la page racine). 

#### Exigences  

Pour compiler ce logiciel, il est nécessaire d'installer la bibliothèque cURL.
Sur les systèmes basés sur Debian : **apt install libcurl4-openssl-dev**

#### Compilation

**g++ -Wall -pipe -Ofast -std=c++11 main.cpp query.cpp timer.cpp -o wikiLinking -lpthread -lcurl**

#### Exemples

Quelques exemples sont disponibles ici : https://github.com/nmrr/wikiLinking---samples
