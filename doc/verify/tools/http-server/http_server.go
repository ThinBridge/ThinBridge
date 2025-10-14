package main

import (
	"flag"
	"fmt"
	"net/http"
	"path/filepath"
)

func customFileServer(fs http.FileSystem) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		path := r.URL.Path
		ext := filepath.Ext(path)

		mimeTypes := map[string]string{
			".js":   "application/javascript; charset=UTF-8",
			".mjs":  "application/javascript; charset=UTF-8",
			".css":  "text/css; charset=UTF-8",
			".html": "text/html; charset=UTF-8",
			".json": "application/json; charset=UTF-8",
		}

		if mimeType, ok := mimeTypes[ext]; ok {
			w.Header().Set("Content-Type", mimeType)
		}

		http.FileServer(fs).ServeHTTP(w, r)
	})
}

func main() {
	var (
		root = flag.String("root", "web/", "root path")
	)
	flag.Parse()

	fs := http.Dir(*root)
	http.Handle("/", customFileServer(fs))

	err := http.ListenAndServe(":8080", nil)
	if err != nil {
		fmt.Printf("ERROR : %s", err)
	}
}
