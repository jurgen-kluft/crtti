package crttr

import (
	denv "github.com/jurgen-kluft/ccode/denv"
	ccore "github.com/jurgen-kluft/ccore/package"
	cunittest "github.com/jurgen-kluft/cunittest/package"
)

// GetPackage returns the package object of 'crttr'
func GetPackage() *denv.Package {
	// Dependencies
	cunittestpkg := cunittest.GetPackage()
	ccorepkg := ccore.GetPackage()

	// The main (crttr) package
	mainpkg := denv.NewPackage("crttr")
	mainpkg.AddPackage(cunittestpkg)
	mainpkg.AddPackage(ccorepkg)

	// 'crttr' library
	mainlib := denv.SetupDefaultCppLibProject("crttr", "github.com\\jurgen-kluft\\crttr")
	mainlib.Dependencies = append(mainlib.Dependencies, ccorepkg.GetMainLib())

	// 'crttr' unittest project
	maintest := denv.SetupDefaultCppTestProject("crttr"+"_test", "github.com\\jurgen-kluft\\crttr")
	maintest.Dependencies = append(maintest.Dependencies, cunittestpkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, ccorepkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, mainlib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddUnittest(maintest)
	return mainpkg
}
