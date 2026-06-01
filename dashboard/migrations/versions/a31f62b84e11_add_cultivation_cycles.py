"""add cultivation cycles

Revision ID: a31f62b84e11
Revises: 943d8c684402
Create Date: 2026-04-27 00:00:00.000000

"""
from alembic import op
import sqlalchemy as sa


# revision identifiers, used by Alembic.
revision = 'a31f62b84e11'
down_revision = '943d8c684402'
branch_labels = None
depends_on = None


def upgrade():
    op.create_table(
        'cultivation_cycles',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('name', sa.String(length=120), nullable=False),
        sa.Column('notes', sa.Text(), nullable=True),
        sa.Column('status', sa.String(length=20), nullable=False),
        sa.Column('started_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=False),
        sa.Column('ended_at', sa.DateTime(timezone=True), nullable=True),
        sa.Column('created_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=False),
        sa.Column('updated_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=False),
        sa.Column('created_by_id', sa.Integer(), nullable=True),
        sa.ForeignKeyConstraint(['created_by_id'], ['users.id']),
        sa.PrimaryKeyConstraint('id')
    )
    with op.batch_alter_table('cultivation_cycles', schema=None) as batch_op:
        batch_op.create_index(batch_op.f('ix_cultivation_cycles_ended_at'), ['ended_at'], unique=False)
        batch_op.create_index(batch_op.f('ix_cultivation_cycles_started_at'), ['started_at'], unique=False)
        batch_op.create_index(batch_op.f('ix_cultivation_cycles_status'), ['status'], unique=False)


def downgrade():
    with op.batch_alter_table('cultivation_cycles', schema=None) as batch_op:
        batch_op.drop_index(batch_op.f('ix_cultivation_cycles_status'))
        batch_op.drop_index(batch_op.f('ix_cultivation_cycles_started_at'))
        batch_op.drop_index(batch_op.f('ix_cultivation_cycles_ended_at'))

    op.drop_table('cultivation_cycles')
