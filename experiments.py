import sys
import os

# print 'Number of arguments:', len(sys.argv), 'arguments.'
# print 'Argument List:', str(sys.argv)

class Run_Experiments():
    def __init__(self, argv):
        argc = len(argv)
        algorithms=["difference"]
        # if(argc>2):
        #     algorithm=argv[2]
        percentages=[80]
        # graphs = ["com-amazon.txt", "dblp-tang.txt"]
        graphs = ["ca-GrQc-processed.txt", "graph_ic.inf", "epinions.txt"]
        self.non_target_maps = dict()

        # self.non_target_maps["com-youtube.txt"] = {80:["com-youtube.txt_1_10_80_611655_1.txt", "com-youtube.txt_1_10_80_572663_1.txt", "com-youtube.txt_1_10_80_706242_1.txt"]}
        # self.non_target_maps["com-amazon.txt"] = {80:["com-amazon.txt_1_10_80_813317_1.txt", "com-amazon.txt_1_10_80_202776_1.txt", "com-amazon.txt_1_10_80_236196_1.txt"]}
        # self.non_target_maps["dblp-tang.txt"] = {80:["dblp-tang.txt_1_10_80_755447_1.txt", "dblp-tang.txt_1_10_80_797251_1.txt", "dblp-tang.txt_1_10_80_938784_1.txt"]}

        for graph_file in graphs:
            for labelMethod in range(0,2):
                self.run_difference_budget_experiments(graph_file, percentages, labelMethod)
                # self.run_tim_budget_experiments(graph_file, percentages, labelMethod)


    def add_ntfile_if_needed(self, graph_file, percentage, command):
        all_commands = [command, command, command]
        if graph_file not in self.non_target_maps:
            return all_commands
        if percentage not in self.non_target_maps[graph_file]:
            return all_commands
        nt_files = self.non_target_maps[graph_file][percentage]
        for i in range(0,len(nt_files)):
            c = all_commands[i]
            c = "{} --ntfile {}".format(c, nt_files[i])
            all_commands[i] = c
        return all_commands

    def run_difference_budget_experiments(self, graph_file, percentages=None, labelMethod=0):
        algorithm="difference"
        for p in percentages:
            for approximation in [4,5]:
                budget = 5
                while budget<=20:
                    unformatted_command = "./influence --algorithm {} -b {} --graph {} --percentage {} --approximation {}  --labelMethod {}"
                    command = unformatted_command.format(algorithm, budget, graph_file, p, approximation, labelMethod)
                    os.system(command)
                    os.system(command)
                    os.system(command)
                    # print command
                    budget+=5

    def run_timtim_budget_experiments(self, graph_file, threshold, percentages=None):
        algorithm="timtim"
        for p in percentages:
            budget = 20
            while budget<=20:
                unformatted_command = "./influence --algorithm {} -b {} --threshold {} --graph {} --percentage {}"

                command = unformatted_command.format(algorithm, budget, threshold, graph_file, p)
                all_commands = self.add_ntfile_if_needed(graph_file, p, command)
                for c in all_commands:
                    os.system(c)

                budget+=2

    def run_baseline_budget_experiments(self, graph_file, threshold, percentages=None):
        algorithm="baseline"
        for p in percentages:
            budget = 20
            while budget<=20:
                unformatted_command = "./influence --algorithm {} -b {} --threshold {} --graph {} --percentage {}"

                command = unformatted_command.format(algorithm, budget, threshold, graph_file, p)
                all_commands = self.add_ntfile_if_needed(graph_file, p, command)
                for c in all_commands:
                    os.system(c)
                budget+=2

    def run_tim_budget_experiments(self, graph_file, percentages=None, labelMethod=0):
        algorithm = "tim"
        for p in percentages:
            budget = 5
            while budget<=20:
                unformatted_command = "./influence --algorithm {} -b {} --graph {} --percentage {} --labelMethod {}"
                command = unformatted_command.format(algorithm, budget, graph_file, p, labelMethod)
                os.system(command)
                os.system(command)
                os.system(command)
                # print command
                budget+=5

    def run_threshold_experiments(self, graph_file, algorithm, percentages, budget=1000):
        for p in percentages:
            threshold=0
            while(threshold<=2000):
                unformatted_command = "./influence --algorithm {} -b {} --threshold {} --graph {} --percentage {}"
                command = unformatted_command.format(algorithm, budget, threshold, graph_file, p)
                all_commands = self.add_ntfile_if_needed(graph_file, p, command)
                for c in all_commands:
                    os.system(c)
                    # print c
                threshold+=20

if(__name__=="__main__"):
    Run_Experiments(sys.argv)
