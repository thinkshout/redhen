<?php

namespace Drupal\redhen_connection\Plugin\Action;

use Drupal\Core\Entity\DependencyTrait;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\redhen_connection\Entity\ConnectionRole;
use Drupal\views_bulk_operations\Action\ViewsBulkOperationsActionBase;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Change the role of a Redhen Connection.
 *
 * @Action(
 *   id = "connection_change_role_action",
 *   label = @Translation("Change the role for the selected connection(s)"),
 *   type = "redhen_connection"
 * )
 */
class ChangeConnectionRole extends ViewsBulkOperationsActionBase implements ContainerFactoryPluginInterface {
  use DependencyTrait;

  /**
   * The connection role entity type.
   *
   * @var \Drupal\Core\Entity\EntityTypeInterface
   */
  protected $entityType;

  /**
   * Indicates if there is a connection type without the specified role.
   *
   * @var int
   */
  protected $mismatch;

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, EntityTypeInterface $entity_type) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->entityType = $entity_type;
    $this->mismatch = 0;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('entity_type.manager')->getDefinition('redhen_connection')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return [
      'role' => '',
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function buildConfigurationForm(array $form, FormStateInterface $form_state) {
    $rids = \Drupal::entityQuery('redhen_connection_role')->execute();
    $role_objects = ConnectionRole::loadMultiple($rids);
    $roles = [];
    foreach ($role_objects as $rid => $role_object) {
      $roles[$rid] = $role_object->label();
    }
    $form['role'] = [
      '#type' => 'radios',
      '#title' => t('Role'),
      '#options' => $roles,
      '#default_value' => '',
      '#required' => TRUE,
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitConfigurationForm(array &$form, FormStateInterface $form_state) {
    $this->configuration['role'] = $form_state->getValue('role');
  }

  /**
   * {@inheritdoc}
   */
  public function calculateDependencies() {
    if (!empty($this->configuration['role'])) {
      $prefix = $this->entityType->getConfigPrefix() . '.';
      $this->addDependency('config', $prefix . $this->configuration['role']);
    }
    return $this->dependencies;
  }

  /**
   * {@inheritdoc}
   */
  public function access($object, AccountInterface $account = NULL, $return_as_object = FALSE) {
    /** @var \Drupal\redhen_connection\ConnectionInterface $object */
    $access = $object->access('update', $account, TRUE);
    return $return_as_object ? $access : $access->isAllowed();
  }

  /**
   * {@inheritdoc}
   */
  public function executeMultiple(array $objects) {
    foreach ($objects as $entity) {
      $this->execute($entity);
    }

    // If there are mismatched roles/connection_types provide a warning.
    if ($this->mismatch) {
      $mismatch_plural = "@count connections could not be updated because the selected role was not associated with the connection type.";
      $mismatch_singular = "@count connection could not be updated because the selected role was not associated with the connection type.";
      $message = \Drupal::translation()->formatPlural($this->mismatch, $mismatch_singular, $mismatch_plural, ['@count' => $this->mismatch]);
      \Drupal::messenger()->addWarning($message);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function execute($connection = NULL) {
    $role = $this->configuration['role'];
    // Skip changing the role for the connection if already present,
    // OR if the role is not available for the given connection.
    $role_storage = \Drupal::service('entity_type.manager')->getStorage('redhen_connection_role');
    $roles = array_keys($role_storage->loadByProperties(['connection_type' => $connection->getType()]));
    if ($connection !== FALSE && $connection->get('role')->getString() != $role) {
      if (in_array($role, $roles)) {
        $connection->original = clone $connection;
        $connection->set('role', $role);
        $connection->save();
      }
      else {
        // Increment mismatch value to indicate a role could not be applied.
        $this->mismatch++;
      }
    }
  }

}
