#include <dynamixel_workbench_toolbox/dynamixel_workbench.h>
#include <program_rekam_gerak/lib_alfan.h>
#include <algorithm>

#include <ryml.hpp>
#include <ryml_std.hpp>


RekamGerakHelper rekamGerakHelper;
string file_name;
string path_to_file;

std::string read_yaml_file(const std::string &path_to_file);
void createNewSequenceYAMLFile(ryml::NodeRef &sequences_of_gerak_tari);
void addNewRecordedMotionFrameYAMLFile(int new_motion_frame_recorded);
void breakCurrentSequenceYAMLFile();
void deleteLastRecordedMotionFrameYAMLFile();

// Structure to hold comment info
struct Comment {
    int line_num;
    int primary_comment_pos;
    int additional_comment_pos;
};

vector<Comment> comments;


void updateYAMLFile(string path, ryml::Tree yaml_tree) {
    // Serialize and write back to file
    ofstream out_file(path_to_file);
    if (!out_file) {
        cerr << "Error: Cannot write to YAML file!" << endl;
        return;
    }

    std::string updated_yaml;
    ryml::emitrs_yaml<std::string>(yaml_tree, &updated_yaml);

    size_t name_pos = 0;
    bool first_found = false;
    int indentation = 4;

    while ((name_pos = updated_yaml.find("- name:", name_pos)) != std::string::npos) {
        if (first_found) {
            updated_yaml.insert(name_pos-indentation, "\n");  // Insert a blank line before the second+ occurrence
            name_pos += 1+indentation;  // Move past the inserted newline
        } else {
            first_found = true;  // Mark that the first occurrence is found
        }
        name_pos += 7+indentation;  // Move past "- name:"
    }

    // out_file << updated_yaml;
    // out_file.close();

    std::string final_yaml = updated_yaml;

    std::istringstream modified_stream(final_yaml);
    std::vector<std::string> modified_lines;
    std::string line;
    int line_number = 0;

    while (std::getline(modified_stream, line)) {
        if (comments.size() > 0) {
            if (line_number == comments.begin()->line_num) {
                // printf("Comment line_num: %d, primary pos: %d, additional pos: %d\n", comments.begin()->line_num, comments.begin()->primary_comment_pos, comments.begin()->additional_comment_pos);
                if (comments.begin()->primary_comment_pos != -1) {
                    line.insert(comments.begin()->primary_comment_pos, "# ");
                }
                if (comments.begin()->additional_comment_pos != -1) {
                    if (comments.begin()->primary_comment_pos != -1) {
                        line.insert(comments.begin()->additional_comment_pos + 2, "# ");
                    } else {
                        line.insert(comments.begin()->additional_comment_pos, "# ");
                    }
                }
                comments.erase(comments.begin());
            }
        }
        modified_lines.push_back(line);
        line_number++;
    }
    
    std::ostringstream modified_yaml;
    for (int i = 0; i < modified_lines.size(); i++) {
        modified_yaml << modified_lines[i] << "\n";
    }

    out_file << modified_yaml.str();
    out_file.close();

    comments.clear();
}

void terminalRekamGerak() {
    printf("'q' - Quit\n");
    printf("'r' - Rekam Gerak\n");
    printf("'b' - Break (buat sequence gerak baru)\n");
    printf("'c' - Cancel (hapus rekaman gerak/motion frame terakhir)\n");

    if (TerminalHelper::key_pressed == 'r') {
          cout << "Rekam gerak..." << endl;
          string filename = FileManager::generateFilename();

          // Tangan Kanan
          rekamGerakHelper.readMX28(21);
          rekamGerakHelper.readXL320(22);
          rekamGerakHelper.readXL320(23);
          rekamGerakHelper.readXL320(24);
          rekamGerakHelper.readXL320(25);
          rekamGerakHelper.readXL320(26);

          // Tangan Kiri
          rekamGerakHelper.readMX28(31);
          rekamGerakHelper.readXL320(32);
          rekamGerakHelper.readXL320(33);
          rekamGerakHelper.readXL320(34);
          rekamGerakHelper.readXL320(35);
          rekamGerakHelper.readXL320(36);

          FileManager::createFile(filename, fileDataTxt, fileDataJson);
          addNewRecordedMotionFrameYAMLFile(counter_rekam_gerak - 1);

          printf("\n'q' - Quit\n'd' - Default\n");
          printf("'r' - Rekam Gerak\n");
          printf("'b' - Break (buat sequence gerak baru)\n");
          printf("'c' - Cancel (hapus rekaman gerak/motion frame terakhir)\n");
    }
    else if (TerminalHelper::key_pressed == 'b') {
        // Break
        cout << "Membuat sequence gerakan baru..." << endl;
        breakCurrentSequenceYAMLFile();
    }
    else if (TerminalHelper::key_pressed == 'c') {
        // Cancel
        cout << "Membatalkan rekaman terakhir..." << endl;
        deleteLastRecordedMotionFrameYAMLFile();
    }
}

std::string read_yaml_file(const std::string &path_to_file) {
    std::ifstream file_yaml(path_to_file, std::ios::in | std::ios::binary);
    if (!file_yaml) {
        std::cerr << "Error: Cannot open YAML file!" << std::endl;
        return "";
    }

    std::ostringstream buffer;
    std::string line;
    std::string yaml_content;
    int line_number = 0;
    int length = 0;

    while (std::getline(file_yaml, line)) {
        bool comment_found = false;
        bool char_found = false;

        for (int i = 0; i < line.size(); i++) {
            if (line[i] == ' ') {
                continue;
            }
            if (line[i] != '#') {
                if (line[i] != ' ') {
                    if (!char_found) {
                        char_found = true;
                    }
                }
            }

            if (!char_found) {
                if (line[i] == '#') {   // menandakan bahwa ini comment utama
                    Comment comment;
                    comment.line_num = line_number;
                    comment.primary_comment_pos = i;
                    comment_found = true;
                    comments.push_back({line_number, i, -1});
                    // printf("[Line %d][Pos %d][Found at %d] %s\n", line_number, length, i, line.c_str());

                    line.erase(i, 2);  // hapus comment & spasi setelahnya
                }
            }
            else {
                if (line[i] == '#') {   // menandakan bahwa ini comment tambahan di akhir
                    int curr_comment_size = comments.size();
                    if (curr_comment_size > 0) {
                        Comment latestComment = comments[comments.size()-1];
                        if (latestComment.line_num == line_number) {
                            latestComment.additional_comment_pos = i;
                            comments[comments.size()-1] = latestComment;
                            comment_found = true;
                            line.erase(i, 2);  // hapus comment & spasi setelahnya
                        }
                        else {
                            comment_found = true;
                            comments.push_back({line_number, -1, i});
                            line.erase(i, 2);  // hapus comment & spasi setelahnya
                        }
                    }
                    else {
                        comment_found = true;
                        comments.push_back({line_number, -1, i});
                        line.erase(i, 2);  // hapus comment & spasi setelahnya
                    }
                    // printf("[Line %d][Pos %d][Found at %d] %s\n", line_number, length, i, line.c_str());
                }
            }
        }
        

        length += line.length();
        yaml_content += line + "\n";
        line_number++;
    }

    file_yaml.close();

    return yaml_content;
}

void createYAMLFile(string name) {
    file_name = name;
    replace(file_name.begin(), file_name.end(), ' ', '_');
    if (!fs::exists(BASE_PATH + string("config_tari"))) {
        fs::create_directory(BASE_PATH + string("config_tari/"));
    }

    path_to_file = BASE_PATH + string("config_tari/") + file_name + ".yaml";
    if (fs::exists(path_to_file)) {
        return;
    }

    ofstream fileYAML(path_to_file);

    fileYAML << "gerak_tari_sequences:\n";
    fileYAML << "  subfolder: " << name << "\n";

    fileYAML.close();
}

void createNewSequenceYAMLFile(ryml::NodeRef &sequences_of_gerak_tari) {
    auto new_sequence = sequences_of_gerak_tari.append_child();
    new_sequence.change_type(ryml::MAP);
    new_sequence["name"] << file_name + " " + to_string(sequences_of_gerak_tari.num_children());
    new_sequence["is_walking"] << "false";
    new_sequence["speeds"] << 5;
    new_sequence["times"] << 500;
}

void addNewRecordedMotionFrameYAMLFile(int new_motion_frame_recorded) {
    std::string yaml_content = read_yaml_file(path_to_file);

    // printf("yaml_content: %s\n", yaml_content.c_str());

    ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(yaml_content));
    ryml::NodeRef root = tree.rootref();
    if (!root.has_child("gerak_tari_sequences")) {
        cerr << "Error: Cannot find gerak_tari_sequences in YAML file!" << endl;
        return;
    }
    
    auto gerak_tari_sequences = root["gerak_tari_sequences"];
    if (!gerak_tari_sequences.has_child("sequences")) {
        auto new_sequences = gerak_tari_sequences.append_child();  // Add a new child
        new_sequences.change_type(ryml::SEQ);  // Ensure it's a sequence
        new_sequences.set_key("sequences");  // Set the key
    }

    auto sequences = root["gerak_tari_sequences"]["sequences"];

    cout << "Number of sequences: " << sequences.num_children() << endl;
    
    // Ensure there is at least one sequence
    if (sequences.num_children() == 0) {
        cerr << "Error: No sequences found. Creating a new sequence..." << endl;
        createNewSequenceYAMLFile(sequences);
    }

    // Get the last sequence
    auto last_sequence = sequences[sequences.num_children() - 1];

    // Ensure motion_frames exists
    if (!last_sequence.has_child("motion_frames")) {
        auto motion_frames = last_sequence.append_child();
        motion_frames.set_key("motion_frames");
        motion_frames.change_type(ryml::SEQ);  // Now it's explicitly a sequence
    }

    auto motion_frames = last_sequence["motion_frames"];

    // Append new motion frame
    auto new_frame = motion_frames.append_child();
    new_frame.change_type(ryml::MAP); // Ensure it's a key-value pair
    new_frame["tangan"] << new_motion_frame_recorded;
    
    updateYAMLFile(path_to_file, tree);

    cout << "Successfully added a new motion frame to YAML." << endl;
}

void breakCurrentSequenceYAMLFile() {
    std::string yaml_content = read_yaml_file(path_to_file);
    ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(yaml_content));
    ryml::NodeRef root = tree.rootref();
    if (!root.has_child("gerak_tari_sequences")) {
        cerr << "Error: Cannot find gerak_tari_sequences in YAML file!" << endl;
        return;
    }

    auto gerak_tari_sequences = root["gerak_tari_sequences"];
    if (!gerak_tari_sequences.has_child("sequences")) {
        cerr << "Error: Cannot find sequences in YAML file!" << endl;
        return;
    }

    auto sequences = root["gerak_tari_sequences"]["sequences"];
    createNewSequenceYAMLFile(sequences);

    updateYAMLFile(path_to_file, tree);
}

void deleteLastRecordedMotionFrameYAMLFile() {
    ifstream file_yaml(path_to_file);
    if (!file_yaml) {
        cerr << "Error: Cannot open YAML file!" << endl;
        return;
    }

    string yaml_content((istreambuf_iterator<char>(file_yaml)), istreambuf_iterator<char>());
    file_yaml.close();

    ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(yaml_content));
    ryml::NodeRef root = tree.rootref();
    if (!root.has_child("gerak_tari_sequences")) {
        cerr << "Error: Cannot find gerak_tari_sequences in YAML file!" << endl;
        return;
    }
    if (!root["gerak_tari_sequences"].has_child("sequences")) {
        cerr << "Error: Cannot find sequences in YAML file!" << endl;
        return;
    }

    auto sequences = root["gerak_tari_sequences"]["sequences"];
    if (sequences.num_children() == 0) {
        cerr << "Error: No sequences found in YAML file!" << endl;
        return;
    }

    auto last_sequence = sequences[sequences.num_children() - 1];
    if (!last_sequence.has_child("motion_frames")) {
        cerr << "Error: No motion_frames found in YAML file!" << endl;
        return;
    }
    auto motion_frames = last_sequence["motion_frames"];

    motion_frames.remove_child(motion_frames.num_children() - 1);
    FileManager::deleteLatestRecord();
    
    // kondisi ketika motion_frames kosong, maka hapus sequence terakhir sekalian
    if (motion_frames.num_children() == 0) {
        sequences.remove_child(sequences.num_children() - 1);
    }

    updateYAMLFile(path_to_file, tree);

    cout << "Successfully deleted the last motion frame." << endl;
}


int main() {
    rekamGerakHelper.init();

    cout << "Masukkan nama Gerak Tari: ";
    string subfolder;
    getline(cin, subfolder);
    printf("Subfolder Gerak Tari: %s\n", subfolder.c_str());

    FileManager::setSubfolder(subfolder);

    createYAMLFile(subfolder);

    TerminalHelper::buildTerminalLoop(terminalRekamGerak);

    return 0;
}
