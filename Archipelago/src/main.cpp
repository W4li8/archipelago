#include <gtkmm/application.h>
#include "archipelago-ui.hpp"
/*
#warning "MyCompilerWarning"
#error "MyCompilerError"
*/
// #include <gtkmm/menu.h>





#include "geometry.hpp"
#include <iostream>
auto main(int argc, char *argv[]) -> int {
	Polygon2D test0({{1,0}});
/*	std::cout <<*/ test0.Perimeter() ;//<<' '
/*			  <<*/ test0.Area() ;//<<" ("
/*			  <<*/ test0.Center();//.x <<','<< test0.Center().y <<") "
/*			  <<*/ test0.Distance({0,0}) ;//<< std::endl;
std::cout<<"\n\n1";
	Polygon2D test1({{1,0},{-1, -1}});
/*	std::cout <<*/ test1.Perimeter() ;//<<' '
/*			  <<*/ test1.Area() ;//<<" ("
/*			  <<*/ test1.Center();//.x <<','<< test1.Center().y <<") "
/*			  <<*/ test1.Distance({0,0}) ;//<< std::endl;

std::cout<<"\n\n2";
	Polygon2D test2({{0,0},{0,-1},{1,0}});test2.Perimeter();test2.Area();test2.Center();test2.Distance({0,0});
	// std::cout << test2.Perimeter()<<' '
	// 		  << test2.Area() <<" ("
	// 		  << test2.Center().x <<','<< test2.Center().y <<") "
	// 		  << test2.Distance({0,0}) << std::endl;
std::cout<<"\n\n3";
	Polygon2D test3({{0,0},{-1,-1},{2,0},{-1,1}});//,{-1,-1}});
/*	std::cout <<*/ test2.Perimeter() ;//<<' '
/*			  <<*/ test3.Area() ;//<<" ("
/*			  <<*/ test3.Center();//.x <<','<< test3.Center().y <<") "
/*			  <<*/ test3.Distance({-1,0}) ;//<< std::endl;
std::cout<<sizeof(double)<<' '<<3.40282347e+38F<<' '<<sizeof(3.40282347e+38F);
Coord2D V0{-1,0}, V1{0,-12.3}, V2{1,0};
	// std::cout<<'='<<(V0 += V2);//((V0-V2)^(V1-V2))<<' '<<(V1^V2)<<' '<<(V2^V0)<<'\n';
std::cout<<"\ncontains: "<<test2.Contains({1, 0})<<'\n';


    auto app = Gtk::Application::create(argc, argv);
    ArchipelagoUI window;
    return app->run(window);
}

