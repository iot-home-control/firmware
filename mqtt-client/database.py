from sqlalchemy.ext.declarative import declarative_base
Base = declarative_base()
import sqlalchemy as sqla

class DataPoint(Base):
    __tablename__ = 'data_points'

    id = sqla.Column(sqla.Integer, primary_key=True)
    node_id = sqla.Column(sqla.Integer)
    timestamp = sqla.Column(sqla.DateTime)
    data = sqla.Column(sqla.Integer)
    data_type = sqla.Column(sqla.Integer, sqla.ForeignKey('data_types.id'))
    blob = sqla.Column(sqla.String)

    def __init__(self, node_id, timestamp, data, datatype, blob=None):
        self.node_id = node_id
        self.timestamp = timestamp
        self.data = data
        self.data_type = datatype
        self.blob = blob


class DataType(Base):
    __tablename__ = 'data_types'

    id = sqla.Column(sqla.Integer, primary_key=True)
    name = sqla.Column(sqla.String)
    conversion_factor = sqla.Column(sqla.Float)
    unit = sqla.Column(sqla.String)

    def __init__(self, name, conversion_factor, unit):
        self.name = name
        self.conversion_factor = conversion_factor
        self.unit = unit
