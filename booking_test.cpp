#include <map>
#include <ctime>
#include <string>
#include <vector>
#include <forward_list>
#include <algorithm>
#include <iostream>

class Booking {
public:
  struct PNR {
    std::string m_paxName;
    std::forward_list<std::string> m_itinerary;

    PNR(const std::string& _paxName,
        const std::forward_list<std::string>& _itenariry)
      : m_paxName(_paxName), m_itinerary(_itenariry) {}

    bool operator==(const PNR &rhs) {
      return m_paxName == rhs.m_paxName
          && m_itinerary == rhs.m_itinerary;
    }
  };
  
  void add(std::tm&& _departure_tm, const PNR& _pnr) {
    auto key = std::mktime(&_departure_tm);
    m_PB.emplace(key, _pnr);
  }

  void add_with_check(std::tm &&_departure_tm, const PNR &_pnr) {
    auto key = std::mktime(&_departure_tm);
    auto check_node = m_PB.find(key);

    if (check_node == m_PB.end())
      m_PB.emplace(key, _pnr);
    else if (!(check_node->second == _pnr))
      m_PB.emplace(key, _pnr);
  }

  auto select_before(std::tm&& _departure_tm) {
    auto key = std::mktime(&_departure_tm);
    return std::make_pair(m_PB.cbegin(), m_PB.lower_bound(key));
  }

  auto find_sequence(const std::forward_list<std::string>& _itinerary) {
    std::vector<value_type::const_iterator> result_PNR;

    for (auto it = m_PB.cbegin(); it != m_PB.cend(); it++)
      if (std::search(it->second.m_itinerary.cbegin(), it->second.m_itinerary.cend(),
                      _itinerary.cbegin(), _itinerary.cend()) != it->second.m_itinerary.cend())
        result_PNR.push_back(it);

    return result_PNR;
  }

  using value_type = std::multimap<std::time_t, PNR>;

private:
  value_type m_PB;
};

void output(const Booking::value_type::const_iterator& cit) {
  std::string departure = std::asctime(std::gmtime(&cit->first));
  departure.pop_back();

  std::cout << cit->second.m_paxName << " | " << departure << " | ";
  
  for (const auto &i : cit->second.m_itinerary)
    std::cout << i << " ";

  std::cout << std::endl;
}

int main() {
  Booking data;

  // 1. add bookings
  data.add({0, 45,  6, 26, 5, 120}, Booking::PNR("Alice", {"LHR", "AMS"}));
  data.add({0, 04, 11,  4, 6, 120}, Booking::PNR("Bruce", {"GVA", "AMS", "LHR"}));
  data.add({0, 00, 10,  6, 6, 120}, Booking::PNR("Cindy", {"AAL", "AMS", "LHR", "JFK", "SFO"}));
  data.add({0,  9,  8, 12, 6, 120}, Booking::PNR("Derek", {"AMS", "LHR"}));
  data.add({0, 40, 20, 13, 6, 120}, Booking::PNR("Erica", {"ATL", "AMS", "AAL"}));
  data.add({0, 10,  9, 14, 6, 120}, Booking::PNR("Fred",  {"AMS", "CDG", "LHR"}));

  // 2. select bookings departing before a given time
  auto before_bound = data.select_before({0,  9,  8, 12, 6, 120});
  for (auto it = before_bound.first; it != before_bound.second; it++)
    output(it);

  std::cout << std::endl;

  // 3. select bookings visiting two airports sequentially
  for (const auto &it : data.find_sequence({"AMS", "LHR"}))
    output(it);
}
