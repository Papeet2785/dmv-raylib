gcc main.c -O1 -Wall -std=c99 -Wno-missing-braces -I include -L lib -lraylib -lopengl32 -lgdi32 -lwinmm -o dmv.exe

if ($LASTEXITCODE -eq 0) {
    .\dmv.exe
}