Fontos assetek és elérési útvonalak

Textúrák és sprite-ok
- Hajó sprite: `res/hajo1.png`
  - Használat: Játékos karakter hajóként jelenik meg.
- Lövedék sprite: `res/loves.te.png`
  - Használat: Lövéshez kapcsolódó sprite.
- Gombok képei:
  - `res/newgame.png` – Új játék gomb
  - `res/settings.png` – Beállítások gomb
  - `res/credits.png` – Készítők listája gomb

Háttérképek
- Térkép háttér: `res/mapbg.png`
  - Használat: Térkép nézethez tartozó háttérkép.
- Szoba hátterek:
  - `res/room1.png` – Szoba 1
  - `res/room2.png` – Szoba 2


Példa a fájlok betöltésére

A fájlok betöltése az SDL és a hozzáadott könyvtárak segítségével történik. Példa egy textúra betöltésére:

SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (!loadedSurface) {
        std::cerr << "Kép betöltési hiba! SDL_image Error: " << IMG_GetError() << std::endl;
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (!newTexture) {
            std::cerr << "Textúra létrehozási hiba! SDL Error: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}
