# Documentation de l'API AlhaizBreizh

## Côté serveur

### Option de lancement

Au moment de lancer le script `serveur.c` plusieur options sont disponibles :

| Option | Description |
|--- |--- |
|`-p <port>` ou `--port <port>` | Cette option permet de spécifier le port sur lequel le serveur va écouter les requêtes clientes. Si ce paramètre n'est pas spécifié le port `8080` sera choisi par défaut |
| `--verbose` | Affiche les logs dans la console du serveur ainsi que dans un fichier `logs.txt`. Si ce fichier existe il sera créé sinon le logs s'ajouteront à la suite dans le fichier. |
| `--help` ou `-h` | Affiche la liste des options possibles |

## Côté client

### Se connecter à l'API

Une fois le programme `serveur.c` lancé, pour se connecter à l'API du côté du client il suffit de taper cette commande :  
`telnet site-sae-ubisoufte.bigpapoo.com <port>`
> [!NOTE]
> `<port>` à remplacer par 8080 si aucun port spécifié au démarrage du script `serveur.c` sinon le remplacer par le port spécifié.

### Fonctionnement du programme général

Une fois le programme client lancé, dans le terminal une ligne vous demandant de renseigner votre code API.

Suite à cela un menu avec 5 outils s'offre à vous :
- Si vous voulez avoir la liste des biens que vous possèdez tapez `1`
- Si vous souhaitez voir le calendrier de disponibilité d'un de vos biens tapez `2`
- Si vous préférez régler les périodes d'indisponibilité d'un de vos logement tapez `3`
- Si vous disposez d'une clé privilégiée et que vous souhaitez voir tous les logements présents sur le site tapez `4`
- Si vous voulez quitter le menu tapez `0`

### Fonctionnalités spécifiques

| Fonctionnalité | Utilisation |
|--- |--- |
| Liste des biens que vous possédez | En sélectionnant cette commande, apparait sur votre terminal toutes vos propriétés qui sont à louer sur le site |
| Calendrier des disponibilités | Pour cette opération après avoir listé tous les biens que vous possèdez, il vous demandera de choisir un ID de cette liste. Pour cela c'est très simple il vous suffit de choisir le nombre à côté du nom de votre logement comme ici `4` pour la `(4) villa stylée` |
| Régler la période d'indisponibilité | A RAJOUTER |
Voir tous les biens du site | Si vous avez une clé API admin vous pouvez utiliser cette fonctionnalité qui vous donnera dans le terminal toutes les habitations directement après avoir sélectionné dans le menu général |

### Terminer le script `serveur.c`

Pour terminer le programme il suffit de taper la commande `exit`.
