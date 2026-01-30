#include "ui.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <cstdio> // Required for snprintf

#include "liblvgl/lvgl.h"
#include "global.hpp"

// Ensure this matches the global definition in global.cpp
extern MecanumDrive drive;

// ============================================================================
//                                 GLOBALS
// ============================================================================

// Pointers to PID labels (so we can update them dynamically)
static lv_obj_t* label_kP = nullptr;
static lv_obj_t* label_kI = nullptr;
static lv_obj_t* label_kD = nullptr;

// ============================================================================
//                                PID LOGIC
// ============================================================================

/**
 * Updates the text on the PID screen to match the robot's actual values.
 * Formats: "kP: 5.250" (3 Decimal Places)
 */
static void update_pid_labels() {
    double p, i, d;
    drive.getDrivePID(p, i, d); // Get values from the global object

    char buffer[32];

    // Helper lambda to round to 3 decimal places for display
    auto get_fraction = [](double val) {
        // 1. Multiply by 1000 to move decimals to integers (5.1999 -> 5199.9)
        // 2. Add 0.5 to round to nearest neighbor (5199.9 + 0.5 -> 5200.4)
        // 3. Cast to int (5200) and Modulo 1000 to get just the last 3 digits (200)
        return (int)(std::abs(val) * 1000 + 0.5) % 1000;
    };

    if (label_kP) {
        snprintf(buffer, sizeof(buffer), "kP: %d.%03d", (int)p, get_fraction(p));
        lv_label_set_text(label_kP, buffer);
    }

    if (label_kI) {
        snprintf(buffer, sizeof(buffer), "kI: %d.%03d", (int)i, get_fraction(i));
        lv_label_set_text(label_kI, buffer);
    }

    if (label_kD) {
        snprintf(buffer, sizeof(buffer), "kD: %d.%03d", (int)d, get_fraction(d));
        lv_label_set_text(label_kD, buffer);
    }
}

/**
 * Callback for PID +/- buttons.
 * IDs: 0=P-, 1=P+, 2=I-, 3=I+, 4=D-, 5=D+
 */
static void pid_adjust_cb(lv_event_t* e) {
    int mode = (int)(uintptr_t)lv_event_get_user_data(e);
    
    double p, i, d;
    drive.getDrivePID(p, i, d); // Get LATEST values

    // Adjust values
    switch(mode) {
        case 0: p -= 0.1; break;   // P -
        case 1: p += 0.1; break;   // P +
        case 2: i -= 0.01; break;  // I - (Smaller steps for I)
        case 3: i += 0.01; break;  // I +
        case 4: d -= 0.1; break;   // D -
        case 5: d += 0.1; break;   // D +
    }

    // Safety checks (cannot be negative)
    if(p < 0) p = 0;
    if(i < 0) i = 0;
    if(d < 0) d = 0;

    drive.setDrivePID(p, i, d); // UPDATE values in drive object
    update_pid_labels();        // REFRESH screen
}

/**
 * Helper to create a single PID row: [Button -] [Label] [Button +]
 */
void create_pid_row(lv_obj_t* parent, lv_obj_t** label_ptr, int id_minus, int id_plus, int y_pos) {
    // 1. The Value Label (Centered)
    *label_ptr = lv_label_create(parent);
    lv_obj_align(*label_ptr, LV_ALIGN_TOP_MID, 0, y_pos + 12); // Vertically centered with buttons
    
    // 2. Minus Button (Left)
    lv_obj_t* btn_minus = lv_button_create(parent);
    lv_obj_set_size(btn_minus, 50, 40); // Wider button for easier touch
    lv_obj_align(btn_minus, LV_ALIGN_TOP_MID, -90, y_pos);
    lv_obj_add_event_cb(btn_minus, pid_adjust_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)id_minus);
    
    lv_obj_t* lbl_m = lv_label_create(btn_minus);
    lv_label_set_text(lbl_m, "-");
    lv_obj_center(lbl_m);

    // 3. Plus Button (Right)
    lv_obj_t* btn_plus = lv_button_create(parent);
    lv_obj_set_size(btn_plus, 50, 40);
    lv_obj_align(btn_plus, LV_ALIGN_TOP_MID, 90, y_pos);
    lv_obj_add_event_cb(btn_plus, pid_adjust_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)id_plus);

    lv_obj_t* lbl_p = lv_label_create(btn_plus);
    lv_label_set_text(lbl_p, "+");
    lv_obj_center(lbl_p);
}

void create_pid_screen() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_screen_load(screen);

    // Title
    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, "PID Tuning (Drive)");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Create Rows (P at 50, I at 100, D at 150)
    create_pid_row(screen, &label_kP, 0, 1, 50);
    create_pid_row(screen, &label_kI, 2, 3, 100);
    create_pid_row(screen, &label_kD, 4, 5, 150);

    // Refresh labels immediately
    update_pid_labels();

    // Back Button
    lv_obj_t* back = lv_button_create(screen);
    lv_obj_set_size(back, 100, 40);
    lv_obj_align(back, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    lv_obj_t* blabel = lv_label_create(back);
    lv_label_set_text(blabel, "Back");
    lv_obj_center(blabel);

    lv_obj_add_event_cb(back, [](lv_event_t* e) { 
        create_main_screen(); 
    }, LV_EVENT_CLICKED, nullptr);
}

// ============================================================================
//                               AUTON LOGIC
// ============================================================================

void updateAutonList() {
    if (auton_master_list.empty()) return;

    // 🟢 STEP A: Force color to match FIRST auton in master list
    int first_side = auton_master_list[0].side;

    if (first_side == 0) autonColor = -1;      // Red
    else if (first_side == 1) autonColor = 1;  // Blue
    // if side == 2 (both), keep existing color

    std::string last_selected_name = "";
    if (!auton_list.empty() && current_auton_selection >= 0 && current_auton_selection < auton_list.size()) {
        last_selected_name = auton_list[current_auton_selection].name;
    }

    auton_list.clear();

    // 🟢 STEP B: Build filtered list
    for (auto &a : auton_master_list) {
        if (a.side == 2) {
            auton_list.push_back(a);
        } else if (a.side == 1 && autonColor == 1) {
            auton_list.push_back(a);
        } else if (a.side == 0 && autonColor == -1) {
            auton_list.push_back(a);
        }
    }

    // 🟢 STEP C: Default selection = FIRST auton from master list
    current_auton_selection = 0;

    for (size_t i = 0; i < auton_list.size(); i++) {
        if (auton_list[i].name == auton_master_list[0].name) {
            current_auton_selection = i;
            break;
        }
    }
}



// ============================================================================
//                            GENERAL UI HELPERS
// ============================================================================

lv_obj_t* create_button(lv_obj_t* parent, const char* text, lv_align_t align, int x_ofs, int y_ofs, void (*callback)()) {
    lv_obj_t* btn = lv_button_create(parent);
    lv_obj_set_size(btn, 180, 70); // Standardized size
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

// ============================================================================
//                                MAIN SCREEN
// ============================================================================

void create_main_screen() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_screen_load(screen);

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x222244), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);
    updateAutonList();
    // Generate Auton Button Text
    std::string autonInfo = "Auton Select\n";
    if (auton_list.empty()) {
        autonInfo += "(None)";
    } else {
        if (autonColor == 1) autonInfo += "[BLUE] ";
        else if (autonColor == -1) autonInfo += "[RED] ";
        else autonInfo += "[?] ";

        if (current_auton_selection >= 0 && current_auton_selection < auton_list.size()) {
            autonInfo += auton_list[current_auton_selection].name;
        } else {
            autonInfo += "Unknown";
        }
    }

    // Top Left: Auton Select
    create_button(screen, autonInfo.c_str(), LV_ALIGN_TOP_LEFT, 20, 20, create_auton_color_screen);

    // Top Right: Profiles
    create_button(screen, "Profiles", LV_ALIGN_TOP_RIGHT, -20, 20, create_profiles_screen);

    // Bottom Left: Odometry
    create_button(screen, "Odometry", LV_ALIGN_BOTTOM_LEFT, 20, -20, create_odometry_screen);

    // Bottom Right: PID Tuning
    create_button(screen, "PID Tuning", LV_ALIGN_BOTTOM_RIGHT, -20, -20, create_pid_screen);
}

// ============================================================================
//                            AUTON SELECTION
// ============================================================================

void auton_red_select() {
    autonColor = -1; 
    updateAutonList(); 
    create_auton_screen();
}

void auton_blue_select() {
    autonColor = 1; 
    updateAutonList();
    create_auton_screen();
}

void create_auton_color_screen() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_screen_load(screen);

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x223355), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, "Choose Side");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t* btnRed = create_button(screen, "RED AUTONS", LV_ALIGN_LEFT_MID, 30, 0, auton_red_select);
    lv_obj_t* btnBlue = create_button(screen, "BLUE AUTONS", LV_ALIGN_RIGHT_MID, -30, 0, auton_blue_select);

    // Highlight Current Selection
    lv_obj_t* activeBtn = (autonColor == -1) ? btnRed : (autonColor == 1) ? btnBlue : nullptr;
    if (activeBtn) {
        lv_obj_set_style_border_color(activeBtn, lv_color_hex(0xFFFF00), 0);
        lv_obj_set_style_border_width(activeBtn, 4, 0);
    }

    lv_obj_t* back = lv_button_create(screen);
    lv_obj_set_size(back, 120, 50);
    lv_obj_set_align(back, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(back, -10);

    lv_obj_t* blabel = lv_label_create(back);
    lv_label_set_text(blabel, "Back");
    lv_obj_center(blabel);

    lv_obj_add_event_cb(back, [](lv_event_t* e) { create_main_screen(); }, LV_EVENT_CLICKED, nullptr);
}

void create_auton_screen() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_screen_load(screen);

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x223355), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    // Title
    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, "Select Auton Routine");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Data Labels
    lv_obj_t* name_label = lv_label_create(screen);
    lv_label_set_text(name_label, auton_list[current_auton_selection].name);
    lv_obj_align(name_label, LV_ALIGN_CENTER, 0, -50);

    lv_obj_t* desc_label = lv_label_create(screen);
    lv_label_set_text(desc_label, auton_list[current_auton_selection].description);
    lv_label_set_long_mode(desc_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(desc_label, 200); 
    lv_obj_set_style_text_align(desc_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(desc_label, LV_ALIGN_CENTER, 0, 10);

    // Arrows
    auto create_arrow = [&](const char* symbol, lv_align_t align, int d) {
        lv_obj_t* btn = lv_button_create(screen);
        lv_obj_set_size(btn, 60, 100);
        lv_obj_set_align(btn, align);
        lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
        
        lv_obj_t* l = lv_label_create(btn);
        lv_label_set_text(l, symbol);
        lv_obj_center(l);
        
        lv_obj_add_event_cb(btn, [](lv_event_t* e) {
            lv_obj_t** labels = (lv_obj_t**)lv_event_get_user_data(e);
            // d is hidden in the event user data pointer math (hacky but standard for simple C callbacks)
            // But here we need to capture 'd'. Since this is a lambda, we can't easily pass 'd' via user_data without a struct.
            // Simplified: We will just inline the logic in a normal loop or split functions.
            // For safety, let's revert to the robust style used in profiles.
        }, LV_EVENT_CLICKED, nullptr);
        return btn;
    };

    // --- Arrow Logic Inline ---
    lv_obj_t* left = lv_button_create(screen);
    lv_obj_set_size(left, 80, 150);
    lv_obj_align(left, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_bg_opa(left, LV_OPA_TRANSP, 0);
    lv_obj_t* l_lbl = lv_label_create(left);
    lv_label_set_text(l_lbl, LV_SYMBOL_LEFT);
    lv_obj_center(l_lbl);
    
    lv_obj_add_event_cb(left, [](lv_event_t* e) {
         lv_obj_t** labels = (lv_obj_t**)lv_event_get_user_data(e);
         current_auton_selection--;
         if (current_auton_selection < 0) current_auton_selection = auton_list.size() - 1;
         lv_label_set_text(labels[0], auton_list[current_auton_selection].name);
         lv_label_set_text(labels[1], auton_list[current_auton_selection].description);
    }, LV_EVENT_CLICKED, new lv_obj_t*[2]{name_label, desc_label});

    lv_obj_t* right = lv_button_create(screen);
    lv_obj_set_size(right, 80, 150);
    lv_obj_align(right, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_bg_opa(right, LV_OPA_TRANSP, 0);
    lv_obj_t* r_lbl = lv_label_create(right);
    lv_label_set_text(r_lbl, LV_SYMBOL_RIGHT);
    lv_obj_center(r_lbl);

    lv_obj_add_event_cb(right, [](lv_event_t* e) {
         lv_obj_t** labels = (lv_obj_t**)lv_event_get_user_data(e);
         current_auton_selection++;
         if (current_auton_selection >= (int)auton_list.size()) current_auton_selection = 0;
         lv_label_set_text(labels[0], auton_list[current_auton_selection].name);
         lv_label_set_text(labels[1], auton_list[current_auton_selection].description);
    }, LV_EVENT_CLICKED, new lv_obj_t*[2]{name_label, desc_label});

    // Back
    lv_obj_t* back = lv_button_create(screen);
    lv_obj_set_size(back, 100, 50);
    lv_obj_align(back, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t* blabel = lv_label_create(back);
    lv_label_set_text(blabel, "Back");
    lv_obj_center(blabel);
    lv_obj_add_event_cb(back, [](lv_event_t* e){ create_main_screen(); }, LV_EVENT_CLICKED, nullptr);
}

// ============================================================================
//                            PROFILES SCREEN
// ============================================================================

void create_profiles_screen() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_screen_load(screen);

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x334466), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, "Driver Profiles");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t* name_label = lv_label_create(screen);
    lv_label_set_text(name_label, profile_list[current_profile_selection].name);
    lv_obj_align(name_label, LV_ALIGN_CENTER, 0, -50);

    lv_obj_t* desc_label = lv_label_create(screen);
    lv_label_set_text(desc_label, profile_list[current_profile_selection].description);
    lv_label_set_long_mode(desc_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(desc_label, 200);
    lv_obj_set_style_text_align(desc_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(desc_label, LV_ALIGN_CENTER, 0, 10);

    // Left Arrow
    lv_obj_t* left = lv_button_create(screen);
    lv_obj_set_size(left, 80, 150);
    lv_obj_align(left, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_bg_opa(left, LV_OPA_TRANSP, 0);
    lv_obj_t* l_lbl = lv_label_create(left);
    lv_label_set_text(l_lbl, LV_SYMBOL_LEFT);
    lv_obj_center(l_lbl);
    
    lv_obj_add_event_cb(left, [](lv_event_t* e) {
        lv_obj_t** labels = (lv_obj_t**)lv_event_get_user_data(e);
        current_profile_selection--;
        if (current_profile_selection < 0) current_profile_selection = profile_list.size() - 1;
        lv_label_set_text(labels[0], profile_list[current_profile_selection].name);
        lv_label_set_text(labels[1], profile_list[current_profile_selection].description);
    }, LV_EVENT_CLICKED, new lv_obj_t*[2]{name_label, desc_label});

    // Right Arrow
    lv_obj_t* right = lv_button_create(screen);
    lv_obj_set_size(right, 80, 150);
    lv_obj_align(right, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_bg_opa(right, LV_OPA_TRANSP, 0);
    lv_obj_t* r_lbl = lv_label_create(right);
    lv_label_set_text(r_lbl, LV_SYMBOL_RIGHT);
    lv_obj_center(r_lbl);

    lv_obj_add_event_cb(right, [](lv_event_t* e) {
        lv_obj_t** labels = (lv_obj_t**)lv_event_get_user_data(e);
        current_profile_selection++;
        if (current_profile_selection >= (int)profile_list.size()) current_profile_selection = 0;
        lv_label_set_text(labels[0], profile_list[current_profile_selection].name);
        lv_label_set_text(labels[1], profile_list[current_profile_selection].description);
    }, LV_EVENT_CLICKED, new lv_obj_t*[2]{name_label, desc_label});

    // Back Button
    lv_obj_t* back = lv_button_create(screen);
    lv_obj_set_size(back, 100, 50);
    lv_obj_align(back, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t* blabel = lv_label_create(back);
    lv_label_set_text(blabel, "Back");
    lv_obj_center(blabel);
    lv_obj_add_event_cb(back, [](lv_event_t* e){ create_main_screen(); }, LV_EVENT_CLICKED, nullptr);
}

// ============================================================================
//                            ODOMETRY SCREEN
// ============================================================================

static void update_odom_label(lv_obj_t* label) {
    drive.updateOdometry();
    const auto& p = drive.getPose();
    double heading = imu.get_heading();

    char buf[160];
    std::snprintf(
        buf, sizeof(buf),
        "X: %.2f\nY: %.2f\nHead: %.1f\nVEnc: %.1f\nHEnc: %.1f",
        p.x, p.y, heading,
        vertical_encoder.get_position(),
        horizontal_encoder.get_position()
    );
    lv_label_set_text(label, buf);
}

static void odom_update_timer(lv_timer_t* timer) {
    lv_obj_t* label = static_cast<lv_obj_t*>(timer->user_data);
    update_odom_label(label);
}

void create_odometry_screen() {
    lv_obj_t* screen = lv_obj_create(nullptr);
    lv_screen_load(screen);

    lv_obj_t* label = lv_label_create(screen);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_30, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -20);

    // Initialize and update label continuously while this screen is active.
    update_odom_label(label);
    lv_timer_create(odom_update_timer, 100, label);

    lv_obj_t* back = lv_button_create(screen);
    lv_obj_set_size(back, 100, 50);
    lv_obj_align(back, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t* blabel = lv_label_create(back);
    lv_label_set_text(blabel, "Back");
    lv_obj_center(blabel);
    lv_obj_add_event_cb(back, [](lv_event_t* e){ create_main_screen(); }, LV_EVENT_CLICKED, nullptr);
}
