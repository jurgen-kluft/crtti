package main

import (
	ccode "github.com/jurgen-kluft/ccode"
	cpkg "github.com/jurgen-kluft/crttr/package"
)

func main() {
	if ccode.Init() {

		ccode.GenerateGitIgnore()
		ccode.GenerateEmbedded()
		ccode.GenerateClangFormat()

		ccode.Generate(cpkg.GetPackage())
	}
}
