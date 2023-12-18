# Documentation de l'API AlhaizBreizh

## Côté serveur

### Option de lancement

Au moment de lancer le script `serveur.c` plusieur options sont disponibles :

| Option | Description |
|--- | :-: |
|`-p <port>` ou `--port <port>` | Cette option permet de spécifier le port sur lequel le serveur va écouter les requêtes clientes. >[!NOTE]> Si ce paramètre n'est pas spécifié le port **8080** sera choisi par défaut |
| `--verbose` | Affiche les logs dans la console du serveur ainsi que dans un fichier `logs.txt`. Si ce fichier existe il sera créé sinon le logs s'ajouteront à la suite dans le fichier. |
