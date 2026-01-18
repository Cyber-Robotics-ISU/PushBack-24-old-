#include "ui.hpp"
#include <vector>
#include <string>
#include <cmath>

#include "liblvgl/lvgl.h"

#include "global.hpp"
#include "autons.hpp"  

void updateAutonList() {
    // 1. Save the name of the currently selected auton (if the list isn't empty)
    std::string last_selected_name = "";
    if (!auton_list.empty() && current_auton_selection >= 0 && current_auton_selection < auton_list.size()) {
        last_selected_name = auton_list[current_auton_selection].name;
    }

    auton_list.clear();

    // 2. Rebuild the list based on color
    for (auto &a : auton_master_list) {
        if (a.side == 2) {
            auton_list.push_back(a); // both sides
        } else if (a.side == 1 && autonColor == 1) {
            auton_list.push_back(a); // blue
        } else if (a.side == 0 && autonColor == -1) {
            auton_list.push_back(a); // red
        }
    }

    // 3. Try to find the previous auton in the new list
    current_auton_selection = 0; // Default to 0 (first item) just in case
    
    for (size_t i = 0; i < auton_list.size(); i++) {
        // If we find the name we saved earlier, set the selection to this index
        if (last_selected_name == auton_list[i].name) {
            current_auton_selection = i;
            break; 
        }
    }
}


// clear screen
void clear_screen(lv_obj_t* screen) {
    lv_obj_clean(screen);
}

lv_obj_t* create_button(lv_obj_t* parent, const char* text, lv_align_t align, int x_ofs, int y_ofs, void (*callback)()) {
    lv_obj_t* btn = lv_button_create(parent);
    lv_obj_set_size(btn, 200, 80);
    lv_obj_set_align(btn, align);
    lv_obj_set_pos(btn, x_ofs, y_ofs);

    lv_obj_t* lbl = lv_label_create(btn);
    lv_label_set_text(lbl, text);
    lv_obj_center(lbl);

    lv_obj_add_event(btn, [](lv_event_t* e) {
        auto func = reinterpret_cast<void (*)()>(lv_event_get_user_data(e));
        if (func) func();
    }, LV_EVENT_CLICKED, reinterpret_cast<void*>(callback));

    return btn;
}


void create_main_screen() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_screen_load(screen);

    const int btn_w = 180, btn_h = 80;
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x222244), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    // --- NEW LOGIC: Generate Button Text ---
    std::string autonInfo = "Auton Select\n";
    
    if (auton_list.empty()) {
        autonInfo += "(None)";
    } else {
        // 1. Add Color Side
        if (autonColor == 1) autonInfo += "[BLUE] ";
        else if (autonColor == -1) autonInfo += "[RED] ";
        else autonInfo += "[?] ";

        // 2. Add Auton Name (Safety Check First)
        if (current_auton_selection >= 0 && current_auton_selection < auton_list.size()) {
            autonInfo += auton_list[current_auton_selection].name;
        } else {
            autonInfo += "Unknown";
        }
    }
    // ---------------------------------------

    // Auton Select (Top Left) - Now uses the custom text!
    lv_obj_t* btn_auton = create_button(screen, autonInfo.c_str(),
    LV_ALIGN_TOP_LEFT, 20, 20, create_auton_color_screen);
    lv_obj_set_size(btn_auton, btn_w, btn_h);

    // Profiles (Top Right)
    lv_obj_t* btn_profiles = create_button(screen, "Profiles",
    LV_ALIGN_TOP_RIGHT, -20, 20, create_profiles_screen);
    lv_obj_set_size(btn_profiles, btn_w, btn_h);

    // Odometry (Bottom Left)
    lv_obj_t* btn_odom = create_button(screen, "Odometry",
        LV_ALIGN_BOTTOM_LEFT, 20, -20, create_odometry_screen);
    lv_obj_set_size(btn_odom, btn_w, btn_h);

    // PID Tuning (Bottom Right)
    lv_obj_t* btn_pid = create_button(screen, "PID Tuning",
        LV_ALIGN_BOTTOM_RIGHT, -20, -20, create_pid_screen);
    lv_obj_set_size(btn_pid, btn_w, btn_h);
}


void auton_red_select() {
    autonColor = -1;   // red
    updateAutonList(); // filter the list
    create_auton_screen();
}

void auton_blue_select() {
    autonColor = 1;    // blue
    updateAutonList();
    create_auton_screen();
}

void create_auton_color_screen() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_screen_load(screen);

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x223355), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    // Title
    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, "Choose Auton Side");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_30, LV_PART_MAIN);
    lv_obj_set_align(title, LV_ALIGN_TOP_MID);
    lv_obj_set_y(title, 10);

    // RED Button
    lv_obj_t* btnRed = create_button(screen, "RED AUTONS",
        LV_ALIGN_LEFT_MID, 30, 0, auton_red_select);
    
    // BLUE Button
    lv_obj_t* btnBlue = create_button(screen, "BLUE AUTONS",
        LV_ALIGN_RIGHT_MID, -30, 0, auton_blue_select);

    // --- NEW LOGIC: Highlight Current Selection ---
    if (autonColor == -1) { // If Red is active
        lv_obj_set_style_border_color(btnRed, lv_color_hex(0xFFFF00), 0); // Yellow border
        lv_obj_set_style_border_width(btnRed, 4, 0);
    } 
    else if (autonColor == 1) { // If Blue is active
        lv_obj_set_style_border_color(btnBlue, lv_color_hex(0xFFFF00), 0); // Yellow border
        lv_obj_set_style_border_width(btnBlue, 4, 0);
    }
    // ----------------------------------------------

    // Back button
    lv_obj_t* back = lv_button_create(screen);
    lv_obj_set_size(back, 120, 50);
    lv_obj_set_align(back, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(back, -10);

    lv_obj_t* blabel = lv_label_create(back);
    lv_label_set_text(blabel, "Back");
    lv_obj_center(blabel);

    lv_obj_add_event_cb(back, [](lv_event_t* e) {
        create_main_screen();
    }, LV_EVENT_CLICKED, nullptr);
}


//  Static Callbacks 
// Left button callback
static void left_btn_event_cb(lv_event_t* e) {
    lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);
    current_auton_selection--;
    if (current_auton_selection < 0)
        current_auton_selection = auton_list.size() - 1;
    lv_label_set_text(label, auton_list[current_auton_selection].name);
}

// Right button callback
static void right_btn_event_cb(lv_event_t* e) {
    lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);
    current_auton_selection++;
    if (current_auton_selection >= (int)auton_list.size())
        current_auton_selection = 0;
    lv_label_set_text(label, auton_list[current_auton_selection].name);
}

static void back_btn_event_cb(lv_event_t* e) {
    create_main_screen();
}

// Auton Selector Screen 
void create_auton_screen() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_screen_load(screen);

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x223355), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    // Title
    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, "Auton Selector");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_30, LV_PART_MAIN);
    lv_obj_set_align(title, LV_ALIGN_TOP_MID);
    lv_obj_set_y(title, 10);

    // Auton Name
    lv_obj_t* name_label = lv_label_create(screen);
    lv_label_set_text(name_label, auton_list[current_auton_selection].name);
    lv_obj_set_style_text_font(name_label, &lv_font_montserrat_30, LV_PART_MAIN);
    lv_obj_set_align(name_label, LV_ALIGN_CENTER);
    lv_obj_set_y(name_label, -60);

    // Auton Description
    lv_obj_t* desc_label = lv_label_create(screen);
    lv_label_set_text(desc_label, auton_list[current_auton_selection].description);
    lv_obj_set_style_text_font(desc_label, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_label_set_long_mode(desc_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(desc_label, 200); // wrap text nicely
    lv_obj_set_align(desc_label, LV_ALIGN_CENTER);
    lv_obj_set_y(desc_label, 0);

    // Left button
    lv_obj_t* left = lv_button_create(screen);
    lv_obj_set_size(left, 100, 200);
    lv_obj_set_align(left, LV_ALIGN_LEFT_MID);
    lv_obj_set_style_bg_opa(left, LV_OPA_TRANSP, LV_PART_MAIN);

    lv_obj_add_event_cb(left, [](lv_event_t* e) {
        lv_obj_t** labels = (lv_obj_t**)lv_event_get_user_data(e);

        // update selection
        current_auton_selection--;
        if (current_auton_selection < 0)
            current_auton_selection = auton_list.size() - 1;

        // update both labels
        lv_label_set_text(labels[0], auton_list[current_auton_selection].name);
        lv_label_set_text(labels[1], auton_list[current_auton_selection].description);
    }, LV_EVENT_CLICKED, new lv_obj_t*[2]{name_label, desc_label});

    // Left arrow
    lv_obj_t* left_arrow = lv_label_create(left);
    lv_label_set_text(left_arrow, LV_SYMBOL_LEFT);
    lv_obj_center(left_arrow);

    // Right button
    lv_obj_t* right = lv_button_create(screen);
    lv_obj_set_size(right, 100, 200);
    lv_obj_set_align(right, LV_ALIGN_RIGHT_MID);
    lv_obj_set_style_bg_opa(right, LV_OPA_TRANSP, LV_PART_MAIN);

    lv_obj_add_event_cb(right, [](lv_event_t* e) {
        lv_obj_t** labels = (lv_obj_t**)lv_event_get_user_data(e);

        current_auton_selection++;
        if (current_auton_selection >= (int)auton_list.size())
            current_auton_selection = 0;

        lv_label_set_text(labels[0], auton_list[current_auton_selection].name);
        lv_label_set_text(labels[1], auton_list[current_auton_selection].description);
    }, LV_EVENT_CLICKED, new lv_obj_t*[2]{name_label, desc_label});

    // Right arrow
    lv_obj_t* right_arrow = lv_label_create(right);
    lv_label_set_text(right_arrow, LV_SYMBOL_RIGHT);
    lv_obj_center(right_arrow);

    // Back button
    lv_obj_t* back = lv_button_create(screen);
    lv_obj_set_size(back, 100, 50);
    lv_obj_set_align(back, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(back, -10);

    lv_obj_t* blabel = lv_label_create(back);
    lv_label_set_text(blabel, "Back");
    lv_obj_center(blabel);

    lv_obj_add_event_cb(back, back_btn_event_cb, LV_EVENT_CLICKED, nullptr);
}

//  Static Callbacks (Profiles) 
static void left_profile_event_cb(lv_event_t* e) {
    lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);
    current_profile_selection--;
    if (current_profile_selection < 0)
        current_profile_selection = profile_list.size() - 1;
    lv_label_set_text(label, profile_list[current_profile_selection].name);
}

static void right_profile_event_cb(lv_event_t* e) {
    lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);
    current_profile_selection++;
    if (current_profile_selection >= (int)profile_list.size())
        current_profile_selection = 0;
    lv_label_set_text(label, profile_list[current_profile_selection].name);
}

static void back_profile_event_cb(lv_event_t* e) {
    create_main_screen();
}

//  Profiles Screen 
//  Profiles Screen 
void create_profiles_screen() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_screen_load(screen);

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x334466), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    // Title
    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, "Driver Profiles");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_30, LV_PART_MAIN);
    lv_obj_set_align(title, LV_ALIGN_TOP_MID);
    lv_obj_set_y(title, 10);

    // Current Profile Name
    lv_obj_t* name_label = lv_label_create(screen);
    lv_label_set_text_fmt(name_label, "%s", profile_list[current_profile_selection].name);
    lv_obj_set_style_text_font(name_label, &lv_font_montserrat_30, LV_PART_MAIN);
    lv_obj_set_align(name_label, LV_ALIGN_CENTER);
    lv_obj_set_y(name_label, -60);

    // Profile Description
    lv_obj_t* desc_label = lv_label_create(screen);
    lv_label_set_text(desc_label, profile_list[current_profile_selection].description);
    lv_obj_set_style_text_font(desc_label, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_label_set_long_mode(desc_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(desc_label, 200); // wrap text nicely
    lv_obj_set_align(desc_label, LV_ALIGN_CENTER);
    lv_obj_set_y(desc_label, 10);

    // Left Tap Zone
    lv_obj_t* left = lv_button_create(screen);
    lv_obj_set_size(left, 100, 200);
    lv_obj_set_align(left, LV_ALIGN_LEFT_MID);
    lv_obj_set_style_bg_opa(left, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_add_event_cb(left, [](lv_event_t* e) {
        lv_obj_t** labels = (lv_obj_t**)lv_event_get_user_data(e);

        current_profile_selection--;
        if (current_profile_selection < 0)
            current_profile_selection = profile_list.size() - 1;

        lv_label_set_text(labels[0], profile_list[current_profile_selection].name);
        lv_label_set_text(labels[1], profile_list[current_profile_selection].description);
    }, LV_EVENT_CLICKED, new lv_obj_t*[2]{name_label, desc_label});

    // Left arrow
    lv_obj_t* left_arrow = lv_label_create(left);
    lv_label_set_text(left_arrow, LV_SYMBOL_LEFT);
    lv_obj_center(left_arrow);

    // Right Tap Zone
    lv_obj_t* right = lv_button_create(screen);
    lv_obj_set_size(right, 100, 200);
    lv_obj_set_align(right, LV_ALIGN_RIGHT_MID);
    lv_obj_set_style_bg_opa(right, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_add_event_cb(right, [](lv_event_t* e) {
        lv_obj_t** labels = (lv_obj_t**)lv_event_get_user_data(e);

        current_profile_selection++;
        if (current_profile_selection >= (int)profile_list.size())
            current_profile_selection = 0;

        lv_label_set_text(labels[0], profile_list[current_profile_selection].name);
        lv_label_set_text(labels[1], profile_list[current_profile_selection].description);
    }, LV_EVENT_CLICKED, new lv_obj_t*[2]{name_label, desc_label});

    // Right arrow
    lv_obj_t* right_arrow = lv_label_create(right);
    lv_label_set_text(right_arrow, LV_SYMBOL_RIGHT);
    lv_obj_center(right_arrow);

    // Back Button
    lv_obj_t* back = lv_button_create(screen);
    lv_obj_set_size(back, 100, 50);
    lv_obj_set_align(back, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(back, -10);

    lv_obj_t* blabel = lv_label_create(back);
    lv_label_set_text(blabel, "Back");
    lv_obj_center(blabel);

    lv_obj_add_event_cb(back, back_profile_event_cb, LV_EVENT_CLICKED, nullptr);
}


//  Odometry Screen 
void create_odometry_screen() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_screen_load(screen);

    double x = vertical_encoder.get_position();
    double y = horizontal_encoder.get_position();
    double heading = imu.get_heading();

    lv_obj_t* label = lv_label_create(screen);
    lv_label_set_text_fmt(label, "X: %.1f\nY: %.1f\nHeading: %.1f", x, y, heading);
    lv_obj_set_align(label, LV_ALIGN_CENTER);

    lv_obj_t* back = lv_button_create(screen);
    lv_obj_set_size(back, 100, 50);
    lv_obj_set_align(back, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(back, -10);

    lv_obj_t* blabel = lv_label_create(back);
    lv_label_set_text(blabel, "Back");
    lv_obj_center(blabel);

    lv_obj_add_event(back, [](lv_event_t* e) { create_main_screen(); }, LV_EVENT_CLICKED, nullptr);
}

//  PID Screen 
void create_pid_screen() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_screen_load(screen);

    lv_obj_t* label = lv_label_create(screen);
    lv_label_set_text(label, "PID tuning TBD");
    lv_obj_set_align(label, LV_ALIGN_CENTER);

    lv_obj_t* back = lv_button_create(screen);
    lv_obj_set_size(back, 100, 50);
    lv_obj_set_align(back, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(back, -10);

    lv_obj_t* blabel = lv_label_create(back);
    lv_label_set_text(blabel, "Back");
    lv_obj_center(blabel);

    lv_obj_add_event(back, [](lv_event_t* e) { create_main_screen(); }, LV_EVENT_CLICKED, nullptr);
}
