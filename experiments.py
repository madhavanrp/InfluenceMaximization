import sys
import os

# print 'Number of arguments:', len(sys.argv), 'arguments.'
# print 'Argument List:', str(sys.argv)

class Run_Experiments():
    def __init__(self, argv):
        argc = len(argv)
        graph_file = argv[1]
        algorithms=["difference", "tim"]
        # if(argc>2):
        #     algorithm=argv[2]
        percentages=[70, 80, 90, 100]
        self.run_difference_budget_experiments(graph_file, percentages)
        self.run_tim_budget_experiments(graph_file, percentages)


    def run_difference_budget_experiments(self, graph_file, percentages=None):
        algorithm="difference"
        for p in percentages:
            for approximation in range(0,4):
                for budget in range(1,21):
                    unformatted_command = "./influence --algorithm {} -b {} --graph {} --percentage {} --approximation {} -e"
                    command = unformatted_command.format(algorithm, budget, graph_file, p, approximation)
                    os.system(command)

    def run_tim_budget_experiments(self, graph_file, percentages=None):
        algorithm="tim"
        for p in percentages:
            for budget in range(1,21):
                unformatted_command = "./influence --algorithm {} -b {} --graph {} --percentage {}"
                command = unformatted_command.format(algorithm, budget, graph_file, p)
                os.system(command)


if(__name__=="__main__"):
    Run_Experiments(sys.argv)