// Implementation for ACMOJ 1421 - ACM EXPRESS
// Only allowed headers: iostream, algorithm, and base.hpp
#include <iostream>
#include <algorithm>
#include "base.hpp"

using namespace std;

struct date {
    int year, month, day;
    date() = default;
    date(int y, int m, int d) : year(y), month(m), day(d) {}
};

// Stream input for date: "<year> <month> <day>"
inline istream &operator>>(istream &is, date &dt) {
    return (is >> dt.year >> dt.month >> dt.day);
}

// Compare dates: earlier time is smaller.
inline bool operator<(const date &a, const date &b) {
    if (a.year != b.year) return a.year < b.year;
    if (a.month != b.month) return a.month < b.month;
    return a.day < b.day;
}

// Helper to compute days with simplified calendar: 360d/year, 30d/month
inline long long days_from_origin(const date &d) {
    return 360LL * d.year + 30LL * d.month + d.day;
}

class mail : public object {
protected:
    string postmark;
    date send_date{};
    date arrive_date{};

public:
    mail() = default;
    mail(string _contain_, string _postmark_, date send_d, date arrive_d)
        : object(_contain_), postmark(_postmark_), send_date(send_d), arrive_date(arrive_d) {}

    // virtual overrides
    string send_status(int, int, int) override { return "not send"; }
    string type() override { return "no type"; }
    void print() override {
        object::print();
        cout << "[mail] postmark: " << postmark << '\n';
    }
    void copy(object *o) override {
        contain = reinterpret_cast<mail *>(o)->contain;
        postmark = reinterpret_cast<mail *>(o)->postmark;
        send_date = reinterpret_cast<mail *>(o)->send_date;
        arrive_date = reinterpret_cast<mail *>(o)->arrive_date;
    }
    virtual ~mail() override {}
};

class air_mail : public mail {
protected:
    string airlines;
    date take_off_date{};
    date land_date{};

public:
    air_mail() = default;
    air_mail(string _contain_, string _postmark_, date send_d, date arrive_d,
             date take_off, date land, string _airline)
        : mail(_contain_, _postmark_, send_d, arrive_d), airlines(_airline),
          take_off_date(take_off), land_date(land) {}

    string send_status(int y, int m, int d) override {
        date ask_date(y, m, d);
        if (ask_date < send_date) return "mail not send";
        if (ask_date < take_off_date) return "wait in airport";
        if (ask_date < land_date) return "in flight";
        if (ask_date < arrive_date) return "already land";
        return "already arrive";
    }
    string type() override { return "air"; }
    void print() override {
        mail::print();
        cout << "[air] airlines: " << airlines << '\n';
    }
    void copy(object *o) override {
        mail::copy(o);
        airlines = reinterpret_cast<air_mail *>(o)->airlines;
        take_off_date = reinterpret_cast<air_mail *>(o)->take_off_date;
        land_date = reinterpret_cast<air_mail *>(o)->land_date;
    }
    ~air_mail() override {}
};

class train_mail : public mail {
protected:
    string *station_name{nullptr};
    date *station_time{nullptr};
    int len{0};

public:
    train_mail() = default;
    train_mail(string _contain_, string _postmark_, date send_d, date arrive_d,
               string *sname, date *stime, int station_num)
        : mail(_contain_, _postmark_, send_d, arrive_d), len(station_num) {
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = sname[i];
                station_time[i] = stime[i];
            }
        }
    }

    string send_status(int y, int m, int d) override {
        date q(y, m, d);
        if (q < send_date) return "mail not send";
        if (len == 0) {
            if (q < arrive_date) return "on the way";
            return "already arrive";
        }
        if (q < station_time[0]) return "wait in station";
        for (int i = 0; i + 1 < len; ++i) {
            if (q < station_time[i + 1]) return string("between ") + station_name[i] + " and " + station_name[i + 1];
        }
        if (q < arrive_date) return string("at ") + station_name[len - 1];
        return "already arrive";
    }

    string type() override { return "train"; }
    void print() override {
        mail::print();
        cout << "[train] station_num: " << len << '\n';
    }
    void copy(object *o) override {
        // Copy base fields
        mail::copy(o);
        // Free existing
        delete[] station_name;
        delete[] station_time;
        station_name = nullptr;
        station_time = nullptr;
        len = 0;
        train_mail *src = reinterpret_cast<train_mail *>(o);
        len = src->len;
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = src->station_name[i];
                station_time[i] = src->station_time[i];
            }
        }
    }
    ~train_mail() override {
        delete[] station_name;
        delete[] station_time;
    }
};

class car_mail : public mail {
protected:
    int total_mile{0};
    string driver;

public:
    car_mail() = default;
    car_mail(string _contain_, string _postmark_, date send_d, date arrive_d,
             int mile, string _driver)
        : mail(_contain_, _postmark_, send_d, arrive_d), total_mile(mile), driver(_driver) {}

    string send_status(int y, int m, int d) override {
        date q(y, m, d);
        if (q < send_date) return "mail not send";
        if (q < arrive_date) {
            double total = (double)(days_from_origin(arrive_date) - days_from_origin(send_date));
            double used = (double)(days_from_origin(q) - days_from_origin(send_date));
            if (total < 1e-12) total = 1.0; // safe-guard
            double current_mile = (used / total) * (double)total_mile;
            return to_string(current_mile);
        }
        return "already arrive";
    }
    string type() override { return "car"; }
    void print() override {
        mail::print();
        cout << "[car] driver_name: " << driver << '\n';
    }
    void copy(object *o) override {
        mail::copy(o);
        total_mile = reinterpret_cast<car_mail *>(o)->total_mile;
        driver = reinterpret_cast<car_mail *>(o)->driver;
    }
    ~car_mail() override {}
};

inline void obj_swap(object *&lhs, object *&rhs) { std::swap(lhs, rhs); }

