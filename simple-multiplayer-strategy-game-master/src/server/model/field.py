from threading import Lock
import csv

class Field:
    def __init__(self, id, questionid1, questionid2, questionid3):
        self.id = id
        self.resources = {
            "iron": {
                "solvers": [],
                "question": questionid1,
            },

            "stone": {
                "solvers": [],
                "question": questionid2,
            },

            "wood": {
                "solvers": [],
                "question": questionid3,
            },
        }
        self.lock = Lock()

    def get_question_id(self, resource):
        """Return question's id"""
        return self.resources[resource]['question']

    def add_solver(self, resource, team_id):
        """Add solver of a field"""
        result = False
        with self.lock:
            is_have = len(list(filter(lambda team: team['team_id'] == team_id, self.resources[resource]['solvers']))) > 0

            if not is_have:
                self.resources[resource]['solvers'].append({
                    'team_id': team_id,
                    'time': 30,
                })

                result = True

        return result

    def is_solved(self, resource, team_id):
        """Check whether a team is solved
        :param resource: kind of resource
        :param team_id: team's id
        :return result
        """
        for x in self.resources[resource]['solvers']:
            if team_id == x['team_id']:
                return True
        return False

    def get_solvers(self, resource):
        """Get a list of solvers of a field
        :param resource: a kind of resource
        :return solvers
        """
        solvers = []

        with self.lock:
            solvers = self.resources[resource]['solvers']

        return solvers

    def reduce_time(self, resource):
        """Reduce time counter and return list of teams will have resource
        :param resource: a kind of resource
        :return teams_have_resource: a list of teams will have new resource
        """
        teams_have_resource = []

        with self.lock:
            solvers = self.resources[resource]['solvers']

            for solver in solvers:
                solver['time'] -= 1
                if solver['time'] == -1:
                    solver['time'] = 30
                    teams_have_resource.append(solver['team_id'])

        return teams_have_resource

    @staticmethod
    def get_fields_from_file(file_name='model/fields.csv'):
        """Get a list of fields from file
        :param file_name: file name
        :return fields: a list of fields
        """
        fields = []
        with open(file_name) as csvfile:
            csvreader = csv.reader(csvfile, delimiter=',', quotechar='"')
            for index, row in enumerate(csvreader):
                fields.append(Field(int(row[0]),
                                    int(row[1]),
                                    int(row[2]),
                                    int(row[3])))
        return fields
