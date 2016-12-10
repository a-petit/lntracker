# lntracker

## démystification du sujet

*** convention d'écritures 

opérateurs binaires espacés d'un caractère 
opérateurs unaires suivis d'un espace
opérateurs ternaires proscrits
code justifié à gauche au maximum ie else ommis lorsque cela est possible
procédures explicités par le mot clé void
80 caractères par ligne maximum

*** version 1 : 1 fichier
lignes de textes non vides possédant plusieurs occurences,
suite str. croissante des numéro de ligne auxquels elle se situe
-> hashtable -> key = line, value = occ_array -> insertion en queue
-> nom du fichier stocké en dehors de la table

*** version 2 : 2 fichiers et plus
pour toute ligne de texte non vides appartenant au premier fichier (REFERENCE)
le nombre d'occurences dans ce premier fichier et dans les autres
-> hashtable engendrée par le fichier de réference 
-> key = lines, value = liste de couples (nom, nb occ) càd des cellules

plusieurs options : 
1, stocker le nom pour chaque valeur (hors de question)
2, stocker le nom une unique fois et associer un indice à un nom
dans le premier cas, le nom est répété de nombreuses fois (bad) 
dans le second cas, de l'espace est gaspillé (bad)
3, utiliser un UID qui sera codé sur un entier non signé de taille (éventuellement selectionnable à la compilation) sinon minimale (devrait largement suffire). L'uid peut n'être rien d'autre qu'un tableau qui contient le nom des fichiers.


*** idée employer une structure compatible pour les deux traitements ? Hmm oui

*** options 
-c TYPE : passer les chaines en UPPER / LOWER
-f MOTIF : isalpha, iscntrl, isdigit, isalnum, p ispunct, isspace
-S : affichage dans l'ordre (strcmp) du contenu des lignes

*** paramètres d'affichage
COL_SEPARATOR : '\t' pourrait être ',' comma
OCC_SEPARATOR : ','



## Analyse préalable du problème

Les clé de la table de hashage sont engendrés par le premier fichier. En d'autres termes, sa taille est indéxée sur le nombre de lignes distinctes du texte de référence. Nous allons donc étudier l'évolution de la table de hashage en fonction du ce fichier.

Dans une implantation classique, la taille du fichier est limitée à SIZE_MAX = 65535 caractères. 
L'alphabet choisi est l'ASCII. Il comprend 256 caractères différents. Posons ALPHA_CARD = 256.

Dans la suite de notre analyse, on assimile toute valeur numérique supérieure à $2^{64}$ comme l'infini.

De façon symétrique, on assimile toute valeur inférieur à $10^{-3}$ à $0$.

L'objet de ce programme est de rechercher des occurences répété de lignes dans un fichier texte. Nous nommerons 'motif' ces éléments (on note que chaque motif est une clé dans la table de hashage).

Les attributs de premiers ordre sont :

1. la **taille du fichier** de référence (en octets), autrement dit le nombre de caractères sur l'aphabet ASCII
2. le **nombre de lignes** du fichier de référence

Que nous considérerons dans leur majoration. De ces deux éléments on déduit : 

3. la **longueur moyenne** de chaque ligne, c'est à dire le nombre de caractères par motif

Enfin, dans le but de dégager un ordre d'idée de la distribution des donnée sur un volume important de texte, nous  tenterons fixer certains repères quand aux paramètres suivants :

1. le **nombre de motifs** (nombre d'entrées dans la table)
2. le **nombre d'occurences** (nombre d'entrées dans la cellule)



Les cas extrêmes sont des cas de faible probabilités mais possible. Il est donc intéressant de les dégager (sans trop s'y attarder) afin de révéler certaines majorations, certaines bornes.

Dans la suite la mention *de référence* (ou *du fichier de référence*) sera omise pour alléger la rédaction.



### Pire de cas, meilleur des cas et cas moyen

- La taille du fichier est considéré comme maximale dans l'ensemble de l'analyse.
- Le nombre de lignes est considéré comme
  - **cas extême (a)** : une unique ligne de SIZE_MAX caractères, ou, SIZE_MAX lignes
  - dans les cas moyens, on considèrera des lignes allant de 10 à 80 caractères
- Le nombre de motifs (nombre d'entrées dans la table) :
  - **cas extême (b)** : une unique motif est répété à chaque ligne du fichier
  - dans sa majoration, le *nombre de motifs potentiel* découle directement de la taille et du nombre de lignes.
  - dans les cas moyens, on se propose d'évaluer le système pour plusieurs valeurs intermédiaires
- Le nombre d'occurences des motifs est sans aucun doutes la valeur la plus instable et donc la  plus délicate à approcher. Cela étant, c'est également celle qui nous intéresse le plus.
  - **cas extême (c)** : parmis l'ensemble des motifs potentiels, un unique motif est répété à chaque ligne du fichier. Le nombre d'occurence est alors maximal
  - **cas extrême (d)** : chaque motif distinct (dans la limite du nombre de motifs potentiel). Le nombre d'occurence de chaque motif est alors minimal.
  - **cas maximal pour un élément détaché** : majoré par le nombre de lignes
  - **cas minimal pour un élément détaché** : minoré par 1 de façon constante
  - cas moyens : distribution linéaire, exponentielle, quadratique, aléatoire



**Application numérique**

| car / lignes |         nb lignes | motifs potentiels       | o moy     | o min | o max |
| -----------: | ----------------: | :---------------------- | :-------- | ----- | ----- |
|          $c$ | $n = SIZEMAX / c$ | $m_p = ALPHACARD^c$     | $n / m_p$ | 1     | n     |
|     SIZE_MAX |                 1 | $2^{SIZEMAX} = \infty$  | 0         | 1     | 1     |
|            1 |             65535 | $2^8 =256$              | 255       | 1     | 65535 |
|            2 |             32768 | $2^{16} \simeq 7.10^4$  | 5         | 1     | 32768 |
|            3 |             21845 | $2^{24} \simeq 2.10^6$  | 0.01      |       |       |
|            5 |             13107 | $2^{40} \simeq 10^{12}$ | $10^{-8}$ | 1     | 13107 |
|           10 |              6554 | $2^{80} = \infty$       | 0         | 1     | 6554  |
|           40 |              1639 | $2^{320} = \infty$      | 0         | 1     | 1639  |
|           80 |               820 | $2^{640} = \infty$      | 0         | 1     | 820   |

Il en ressort que le nombre d'occurence moyenne avoisine 0 dès lors que les motifs ont une longueur moyenne supérieur à 3 caractères. Aussi, les nombre d'occurence peut atteindre une valeur très élevée pour les motifs courts.

**Heuristique sur le nombre d'occurences**

Le nombre d'occurences sera majoritairement de faibles valeurs : 1 dans la plus part des cas., puis 2, 3, 4, 5 (de moins en moins fort). Inutile donc de prévoir à l'avance des espaces de stockage de grande taille pour le référencement du nombre d'occurences. Prévoir une structure légère pour ce type de données.

- Liste chainée avec mémorisation de la longueur et pointeur de queue : 

Espace minimal supérieur à celui d'un tableau dynamique

Multiplication des allocations dans un cas maximal

- Tableau dynamique sur le type size_t : 

Espace minimal optimal

Gestion des allocations optimal

Présente comme inconvénient d'être difficilement extensible à un type plus complexe

avantage : accès à la longueur calculé en temps constant



## Analyse des structures de données

- Table de hashage (registre)
  - spécificité : utilisé pour des insertions essentiellement
- Liste des numéros de lignes par fichier
  - données : homogènes, complexes
  - accés : parcours itératif suffisant
- Liste des numéros de lignes
  - requis : accès à la longueur en temps constant
  - données : homogènes, primitive (size_t)
  - accès : parcours itératif
- Liste des motifs
  - requis : tri croissant
  - accès : parcours itératif
- Liste des fichiers
  - données : homogènes (chaines de caractères de longueur variable)
  - accès : parcours itératif 
  - accès à la longueur en temps constant



## Planification

- Traitement des options (récupération)
- Traitement des fichiers (ouverture, parcours, fermeture)
- Module lnscan
- Implantation des structures de données
  - ~~Table de hashage~~ *(registre)*
  - Tableau dynamique  *(liste des occurences, liste des motifs, liste des fichiers, liste des fichiers une occ)*
- Affichage





## Questions

- allocation d'entiers
- fscanf sur les fichiers
- importance d'avoir structures non sloppy ?