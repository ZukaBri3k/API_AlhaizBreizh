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

---

### Terminer le script `serveur.c`

Pour terminer le programme il suffit de taper la commande `exit`.
