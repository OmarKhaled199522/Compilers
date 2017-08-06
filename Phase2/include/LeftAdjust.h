#ifndef LEFTADJUST_H
#define LEFTADJUST_H
#include <bits/stdc++.h>


class LeftAdjust
{
    public:
        LeftAdjust();
        virtual ~LeftAdjust();
        void get_left_recursion (std::vector <std::vector <std::string> > &,
                                                  std::vector <std::string> &);
        void get_left_factoring (std::vector <std::vector <std::string> > &,
                                                  std::vector <std::string> &);

    protected:

    private:
        void adjust_left_recursion (std::vector <std::vector <std::string> > &, std::vector <std::string> &);
        void adjust_left_factoring (std::vector <std::vector <std::string> > &, std::vector <std::string> &);
        bool factor_row (std::vector <std::vector <std::string> > &, std::vector <std::string> &, int);
        std::vector <std::vector <int>> get_similar_start(std::vector <std::vector <std::string> > &, std::vector <std::string> &, int);
        std::vector <std::string> get_string_tokens (std::string);
        std::string get_first_sym (std::string, int &);
        void replace_transitions (std::vector <std::vector <std::string> > &,
                                 std::vector <std::string> &, int, int);
        void remove_immediate_recursion(std::vector <std::vector <std::string> > &,
                                        std::vector <std::string> &non_terms, int &);
        std::string group_elements (std::vector <std::vector <std::string> > &, std::vector <std::string> &,
                             const std::vector <int> &, int, int);
        std::vector <std::vector <std::string> > added_symbols;
        static const std::string epislon;
};

#endif // LEFTADJUST_H
