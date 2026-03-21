#include <math.h>
#include <iostream>
#include <string>

#include <GLFW/glfw3.h> 

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "Compiler.cpp"

std::string queryBuffer = "Hello";
int idx = -1;

Lexer& lexer = *(new Lexer());
Database& db = *(new Database());

void DatabaseWindow() {
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(screenSize.x * 0.5f, screenSize.y));
    
    ImGui::Begin("Database", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    if (ImGui::BeginMenuBar()) {
        for (int i = 0; i < db.getDatabaseData().size(); i++) {
            if (ImGui::MenuItem(&(db.getDatabaseData()[i].getTableName()[0]))) {
                if (idx != i) {
                    idx = i;
                }
            }
        }
        ImGui::EndMenuBar();
    }
    
    // Print table
    if (idx != -1) {
        std::string qryret = "";
        Table& table = db.getDatabaseData()[idx];
        for (int i = 0; i < table.getTableData().size(); i++) {
            for (int j = 0; j < table.getTableData()[i].getRowData().size(); j++) {
                qryret += table.getTableData()[i].getRowData()[j].getValue() + " ";
            }
            qryret += "\n";
        }

        if (qryret.size() != 0) {
            ImGui::Text(&(qryret[0]));
        }
    }

    ImGui::End();
}

void EditorWindow(int buttonHeight = 30) {
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(screenSize.x * 0.5f, 0));
    ImGui::SetNextWindowSize(ImVec2(screenSize.x * 0.5f, screenSize.y));

    ImGui::Begin("Editor", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);

    ImGui::InputTextMultiline(
        "##Editor", 
        &queryBuffer, 
        ImVec2(-FLT_MIN, ImGui::GetWindowHeight() - 2 * buttonHeight), 
        ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CtrlEnterForNewLine
    );

    if (ImGui::Button("Compile Script", ImVec2(150, buttonHeight))) {
        std::cout << "\n[COMPILER INTERCEPT] Executing DFA on:\n";
        std::cout << queryBuffer << "\n";

        lexer.runLexer(db, queryBuffer);
    }

    ImGui::End();
}

GLFWwindow* initWindow() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "MyDB", NULL, NULL);
    glfwMakeContextCurrent(window);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    return window;
}

void Render(GLFWwindow* window) {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DatabaseWindow();
        EditorWindow();

        ImGui::Render();
        
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.15f, 0.15f, 0.15f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

int main(int, char**) {
    Create* create = new Create(db);
    Read* read = new Read(db);
    Update* update = new Update(db);
    Delete* del = new Delete(db);

    create->createTable("EMPLOYEES", vector<string> {"NAME", "SURNAME", "SALARY"});
    create->createTable("WORKERS", vector<string> {"NAME", "SURNAME", "SALARY"});

    Table& table = db.getTable("EMPLOYEES");
    
    update->addRow("EMPLOYEES", vector<string> {"John2", "Doe", "300"});
    del->deleteRow("EMPLOYEES", 2);

    read->readEntireDatabase();

    GLFWwindow* window = initWindow();
    Render(window);

    return 0;
}