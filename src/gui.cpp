#include "gui.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

Gui::Gui(GLFWwindow* window) : window(window) {
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui_ImplOpenGL3_Init("#version 420");
    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui::GetIO().IniFilename = nullptr;
}

Gui::~Gui() {
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void Gui::render(Trajectory& trajectory) {
    // get the GLFW pointer
    GLFWPointer* pointer = (GLFWPointer*)glfwGetWindowUserPointer(window);

    // define some static variables
    static float bindingFactor = BINDINGFACTOR, bondSize = BONDSIZE, atomSizeFactor = ATOMSIZEFACTOR;
    static int subdivisions = SUBDIVISIONS, sectors = SECTORS;
    static bool smooth = SMOOTH;

    // refine functions that recreate the meshes
    auto remeshCylinders = [](int sectors, bool smooth) {
        Geometry::meshes.at("bond") = Mesh::Cylinder(sectors, smooth, "bond"); 
    };
    auto remeshSpheres = [](int subdivisions, bool smooth) {
        for (auto& [symbol, object] : ptable) {
            Geometry::meshes.at(symbol) = Mesh::Icosphere(subdivisions, smooth, symbol);
            Geometry::meshes.at(symbol).setColor(object.color);
        }
    };

    // begin frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // options window
    if (pointer->flags.options) {

        // begin the window
        ImGui::Begin("Options", &pointer->flags.options, ImGuiWindowFlags_AlwaysAutoResize);

        // smooth checkbox
        if (ImGui::Checkbox("Smooth", &smooth)) {
            remeshSpheres(subdivisions, smooth);
            remeshCylinders(sectors, smooth);
        }

        // separator
        ImGui::Separator();

        // mesh options
        if (ImGui::SliderInt("Sphere", &subdivisions, 0, 6)) remeshSpheres(subdivisions, smooth);
        if (ImGui::SliderInt("Cylinder", &sectors, 4, 128)) remeshCylinders(sectors, smooth);
        if (ImGui::SliderFloat("Atom Size Factor", &atomSizeFactor, 0.001, 0.02)) {
            for (auto& molecule : trajectory.getGeoms()) molecule.setAtomSizeFactor(atomSizeFactor);
        }
        if (ImGui::SliderFloat("Bond Size", &bondSize, 0.01, 0.2)) {
            for (auto& molecule : trajectory.getGeoms()) molecule.setBondSize(bondSize);
        }

        //separator
        ImGui::Separator();
        
        // number factors
        if (ImGui::SliderFloat("Binding Factor", &bindingFactor, 0, 0.05f)) {
            for (auto& molecule : trajectory.getGeoms()) molecule.rebind(bindingFactor);
        }

        // separator
        ImGui::Separator();
        
        // lighting options
        ImGui::SliderFloat("Ambient", &pointer->light.ambient, 0, 1);
        ImGui::SliderFloat("Diffuse", &pointer->light.diffuse, 0, 1);
        ImGui::SliderFloat("Specular", &pointer->light.specular, 0, 1);
        ImGui::SliderFloat("Shininess", &pointer->light.shininess, 1, 128);

        // separator
        ImGui::Separator();

        // trajectory options
        ImGui::SliderInt("Frame", &trajectory.getFrame(), 0, trajectory.size() ? trajectory.size() - 1 : 0);
        ImGui::SliderFloat("Timeout", &trajectory.getWait(), 0.001, 16);

        // separator
        ImGui::Separator();
        
        // function buttons
        if (ImGui::Button("Center")) {
            trajectory.moveBy(-trajectory.getGeoms().at(trajectory.getFrame()).getCenter());
        }

        // end the window
        ImGui::End();
    }

    // system window
    if (pointer->flags.system) {

        // begin the window
        ImGui::Begin("Atom Distance Analysis", &pointer->flags.system, ImGuiWindowFlags_AlwaysAutoResize);

        // current geomery objects and plot atom indices
        auto objects = trajectory.getGeoms().at(trajectory.getFrame()).getObjects(); int size = 0;
        for (size_t i = 0; i < objects.size(); i++) if (objects.at(i).name != "bond") size++;
        static int atom1 = 1, atom2 = 2;

        // coordinate plot data and current frame
        static std::vector<float> x, y;
        static int frame = 0;

        // clear if trajectory starts from beginning
        if (frame > trajectory.getFrame()) x.clear(), y.clear();
        frame = trajectory.getFrame();

        // atom index sliders
        if(ImGui::VSliderInt("##Atom 1", ImVec2(15, 255), &atom1, 1, size)) {
            x.clear(), y.clear();
        } ImGui::SameLine();
        if(ImGui::VSliderInt("##Atom 2", ImVec2(15, 255), &atom2, 1, size)) {
            x.clear(), y.clear();
        } ImGui::SameLine();

        // push the distane to the y vector
        if (!pointer->flags.pause || !x.size()) {
            y.push_back(glm::length(objects.at(atom1 - 1).getPosition() - objects.at(atom2 - 1).getPosition()));
            x.push_back(x.size() + 1);
        }

        // plot
        if (ImPlot::BeginPlot("Atom Plot", ImVec2(320, 255), ImPlotFlags_NoTitle | ImPlotFlags_NoLegend)) {
            ImPlot::SetupAxisLimits(ImAxis_X1, x.at(x.size() - 1) - 100, x.at(x.size() - 1), pointer->flags.pause ? ImGuiCond_Once : ImGuiCond_Always);
            ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks);
            ImPlot::SetupAxisLimits(ImAxis_Y1, y.at(0) - 0.5, y.at(0) + 0.5);
            ImPlot::PlotLine("Line", x.data(), y.data(), x.size());
            ImPlot::EndPlot();
        }

        // coordinate table on the same line
        ImGui::SameLine();

        // create the table
        if (ImGui::BeginTable("Atoms", 5, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInner | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg, ImVec2(-1, 255))) {
            ImGui::TableSetupColumn("ID"), ImGui::TableSetupColumn("SM"), ImGui::TableSetupColumn("X");
            ImGui::TableSetupColumn("Y"), ImGui::TableSetupColumn("Z"), ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow(); bool hovering = false;
            for (size_t i = 0; i < objects.size(); i++) {
                if (objects.at(i).name == "bond") continue;
                ImGui::PushID(i); bool selected = 0;
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%d", (int)i + 1);
                ImGui::TableNextColumn();
                ImGui::Text("%s", objects.at(i).name.c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%.3f", objects.at(i).getPosition().x);
                ImGui::TableNextColumn();
                ImGui::Text("%.3f", objects.at(i).getPosition().y);
                ImGui::TableNextColumn();
                ImGui::Text((std::string("%.3f") + (size > 15 ? "  " : "")).c_str(), objects.at(i).getPosition().z);
                ImGui::SameLine(); ImGui::Selectable("##", selected, ImGuiSelectableFlags_SpanAllColumns);
                if (ImGui::IsItemHovered()) {
                    pointer->highlight = i, hovering = true;
                }
                ImGui::PopID();
            }
            if (!hovering) pointer->highlight = -1;
            ImGui::EndTable();
        }

        
        // end the window
        ImGui::End();
    }

    // periodic table window
    if (pointer->flags.ptable) {

        // begin the window
        ImGui::Begin("Periodic Table", &pointer->flags.ptable, ImGuiWindowFlags_AlwaysAutoResize);

        // end the window
        ImGui::End();
    }

    // info window
    if (pointer->flags.info) {
        ImGui::SetNextWindowPos({ 0, 0 }); ImGui::Begin("info", &pointer->flags.info,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoFocusOnAppearing
        );
        ImGui::Text("%.1f", ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // pause window
    if (pointer->flags.pause) {
        ImGui::SetNextWindowPos({ (float)pointer->width - 58, 0 });
        ImGui::Begin("pause", &pointer->flags.pause,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoFocusOnAppearing
        );
        ImGui::Text("PAUSED");
        ImGui::End();
    }

    // export the trajectory to the .xyz format
    if (ImGuiFileDialog::Instance()->Display("Export Molecule", ImGuiWindowFlags_NoCollapse, { 512, 288 })) {
        if (ImGuiFileDialog::Instance()->IsOk()) {

            // setup variables for saving the buffer
            std::ofstream file(ImGuiFileDialog::Instance()->GetFilePathName());

            // iterateover all geoms and write the geometry to the file
            for (auto geom : trajectory.getGeoms()) {
                file << geom.size() << "\ntrajectory\n";
                for (auto object : geom.getObjects()) {
                    if (object.name == "bond") continue;
                    file << object.name << " " << object.getPosition().x << " " << object.getPosition().y;
                    file << " " << object.getPosition().z << "\n";
                }
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // if importing the molecule open file window
    if (ImGuiFileDialog::Instance()->Display("Import Molecule", ImGuiWindowFlags_NoCollapse, { 512, 288 })) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            trajectory = Trajectory::Load(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // if saving the molecule open the window
    if (ImGuiFileDialog::Instance()->Display("Save Molecule", ImGuiWindowFlags_NoCollapse, { 512, 288 })) {
        if (ImGuiFileDialog::Instance()->IsOk()) {

            // setup variables for saving the buffer
            std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string extension = path.substr(path.find_last_of(".") + 1);
            GLint viewport[4]; glGetIntegerv(GL_VIEWPORT, viewport);
            int width = viewport[2], height = viewport[3];
            unsigned char pixels[4 * width * height];

            // read the buffer
            glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

            // flip the image
            for (int i = 0; i < 4 * width; i++) {
                for (int j = 0; j < height / 2; j++) {
                    std::swap(pixels[4 * j * width + i], pixels[4 * (height - j - 1) * width + i]);
                }
            }

            // save the buffer
            if (extension == "png") {
                stbi_write_png(path.c_str(), width, height, 4, pixels, 4 * width);
            } else if (extension == "jpg") {
                stbi_write_jpg(path.c_str(), width, height, 4, pixels, 80);
            } else if (extension == "bmp") {
                stbi_write_bmp(path.c_str(), width, height, 4, pixels);
            } else {
                throw std::runtime_error("Unknown file extension.");
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // render the gui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
