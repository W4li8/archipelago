// #include <bits/stdc++.h>
#include <gtkmm/application.h>

#include "archipelago-ui.hpp"

// #include "graph.hpp"

// std::ostream& operator<<(std::ostream& os, const Node& obj) {
//     os << obj.data <<' '<< obj.level;

//     for(auto& x : obj.Links) {
//         os << "\n----\n"<<*(x->node);
//     }
//     return os;
// }


auto main(int argc, char *argv[]) -> int {
    auto app = Gtk::Application::create(argc, argv);
    ArchipelagoUI window;
	return app->run(window);

    // auto A = std::make_shared<Node>(100, 2);
    // auto a = std::make_shared<Node>(99, 1);
    // auto X = std::make_shared<Link>(7, A);
    // auto b = std::make_shared<Node>(98, 1, std::vector< std::shared_ptr<Link> > ({X}));
    // auto x = std::make_shared<Link>(3.1415, a);
    // auto y = std::make_shared<Link>(2.7182, b);
    // auto c = std::make_shared<Node>(97, 0, std::vector< std::shared_ptr<Link> > ({y, x,X, x}));
    // // Node x{std::make_shared<Node>(9,0, {std::make_shared<Node>({8,1, {}}),std::make_shared<Node>({7,1, {}})})};
    // std::cout<<*c<<'\n'<<'\n'<<'\n';

    // while(1) {
    //     double a, b;
    //     std::cout << "PGCD(a, b): ";
    //     std::cin >> a >> b;
    //     std::cout << "PGCD("<<a<<", "<<b<<") = ";
    //     for(double r{0}; b != 0;) {
    //         r = fmod(a,b);
    //         a = b;
    //         b = r;
    //     }
    //     std::cout <<a<<'\n';
    // }

}

