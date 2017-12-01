from threading import Timer
from model.resource import RESOURCES
from model.interval import VALUE


def check_data(teams, fields, castles, init=False):
    """Update data after 1 second
    :param teams: a list of team
    :param fields: a list of field
    :param castles: a list of castles
    :param init: whether is init (Default value=False)
    :return timer
    """
    timer = Timer(1.0, check_data, [
        teams,
        fields,
        castles,
    ])

    timer.start()

    if not init:
        for field in fields:
            for resource in RESOURCES[:2]:
                teams_have_resource = field.reduce_time(resource)

                for solver in teams_have_resource:
                    teams[solver - 1].add_resource(resource, VALUE[resource])

        for castle in castles:
            resource = RESOURCES[3]

            if castle.owner_id is not None:
                if castle.reduce_gold_delay():
                    teams[castle.owner_id - 1].add_resource(resource, VALUE[resource])

            castle.reduce_block()

    return timer
